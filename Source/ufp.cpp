#include <file.h>
#include <string>
#include <rope>
#include "validateunicode.h"
#include "utfconvert.h"

using namespace std;

typedef wchar_t TCHAR;
typedef std::basic_string<TCHAR> tstring;

class UnicodeTextFile
{
public:
   UnicodeTextFile(FILE* fp);
   ~UnicodeTextFile();
   tstring getLine();
   bool eof() const;
   bool isLE() const;

private:
   unsigned char getByte();

   std::rope buf_;
   CValidateUnicode::FILE_TYPE ftype_;
};

UnicodeTextFile::UnicodeTextFile(FILE* fp)
   : fp_(fp)
   , ftype_(CValidateUnicode::UNKNOWN)
{
   int c;
   bool done = false;
   string b;

   while ((c = fgetc()) != EOF && !done)
   {
      b.push_back((char)c);
   }

   this->ftype_ =
      CValidateUnicode::CheckBom(b.c_str(), b.size());

   switch (type)
   {
      case CValidateUnicode::UTF_8:
         buf_.assign(b.begin() + 3, b.end());
         break;
      case CValidateUnicode::UTF_16LE:
         buf_.assign(b.begin() + 2, b.end());
         break;
      case CValidateUnicode::UTF_16BE:
         buf_.assign(b.begin() + 2, b.end());
         break;
      case CValidateUnicode::UTF_32LE:
      case CValidateUnicode::UTF_32BE:
      default:
         this->buf_.assign(b.begin(), b.end());
         break;
   }

   while ((c = fgetc()) != EOF)
   {
      this->buf_.push_back(c);
   }
}

UnicodeTextFile::~UnicodeTextFile()
{
}

template <class CharType>
CharType getChar(FILE* fp, bool isLE)
{
   CharType rval = 0;

   if (feof(fp)) return rval;

   if (isLE)
   {
      size_t count = 0;
      while (count < sizeof(CharType))
      {
         int c = fgetc(fp);
         if (c == EOF)
         {
            throw "getCharHelper：过早的文件结尾。";
         }
         rval |= ((CharType)c) << (8 * count);
         ++count;
      }
   }
   else
   {
      size_t count = sizeof(CharType) - 1;

      do
      {
         int c = fgetc(fp);
         if (c == EOF)
         {
            throw "getCharHelper： 过早的文件结尾。";
         }
         rval |= ((CharType)c) << (8 * count);
         --count;
      } while (count != 0);
   }

   return rval;
}

bool UnicodeTextFile::isLE() const
{
   return (this->ftype_ == CValidateUnicode::UTF_8 ||
           this->ftype_ == CValidateUnicode::UTF_16LE ||
           this->ftype_ == CValidateUnicode::UTF_32LE ||
           this->ftype_ == CValidateUnicode::UNKNOWN);
}

bool eof() const
{
   return feof(this->fp_);
}

TCHAR UnicodeTextFile::getChar()
{
   return getCharHelper<
}
