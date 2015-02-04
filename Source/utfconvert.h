/*----------------------------------------------------------------------------
                                 utfconvert.h
                                 ------------------------------------------------------------------------------

                                 Utilities for converting to and from various UTF encodings.
                                 Author:    Jim Park
                                 Copyright: 2012

                                 ----------------------------------------------------------------------------*/

#ifndef UTFCONVERT_
#define UTFCONVERT_

namespace detail

    //----------------------------------------------------------------------------
    //                          UTFConverter Template
    //----------------------------------------------------------------------------

    /**
     * The UTFConverter template is a helper template to convert from one UTF
     * encoding to another.
     * <p>
     * These templates are used to resolve the correct encoding algorithm
     * regardless of what type the character type is. For example, char16_t,
     * unsigned short and wchart_t in Windows are actually the same bitwidth and
     * can be used to represent UTF-16. Therefore, the algorithms needed to
     * convert strings of these types that encode UTF-16 to other UTF encodings
     * are the same.
     * <p>
     * @author       Jim Park
     * @copyright    2012
     */

     template <int FromUCS, int ToUCS, typename FromIter, typename To>
struct UTFConverter
{
    // The default template has no operator() defined which will cause
    // a compiler error for unsupported conversions.
};

//----------------------------------------------------------------------------
//                          UTFConverter Template
//----------------------------------------------------------------------------

/**
 * Specialization that will convert UTF-8 to UTF-16.
 * <p>
 * @author       Jim Park
 * @copyright    2012
 */

template <typename FromIter, typename To>
struct UTFConverter < 1, 2, FromIter, To >
{
    bool operator()(FromIter begin, FromIter end, To& dest)
    {
        bool result = true;

        unsigned char code;
        unsigned char second_byte;
        unsigned char third_byte;

        while (begin != end)
        {
            code = (unsigned char)(*begin);

            if (code <= 0x007f)
            {
                dest.push_back(code);
            }
            else if (code >> 5 == 0x6) // 110 prefix
            {
                // 2-byte sequence.
                if (++begin == end)
                {
                    result = false;
                    break;
                }

                dest.push_back(((code & 0x1f) << 6) | (*begin) & 0x3f);
            }
            else if (code >> 4 == 0xe) // 1110 prefix
            {
                // 3-byte sequence
                if (++begin == end)
                {
                    result = false;
                    break;
                }
                second_byte = (unsigned char)(*begin);

                if (++begin == end)
                {
                    result = false;
                    break;
                }

                dest.push_back(
                    ((code & 0x0f) << 12) |
                    ((second_byte & 0x3f) << 6) |
                    ((*begin) & 0x3f));
            }
            else if (code >> 3 == 0x1e) // 11110 prefix
            {
                // 4-byte sequence
                if (++begin == end)
                {
                    result = false;
                    break;
                }
                second_byte = (unsigned char)(*begin);

                if (++begin == end)
                {
                    result = false;
                    break;
                }
                third_byte = (unsigned char)(*begin);

                if (++begin == end)
                {
                    result = false;
                    break;
                }

                // This one needs a surrogate.
                // The following computation gets the uuuuu bytes from The Unicode
                // Standard 5.0, page 103.
                typename To::value_type leading =
                    ((code & 0x7) << 2) | ((second_byte & 0x3f) >> 4);

                // 0xd800 is the mask for the leading surrogate. The uuuuu bytes - 1 is the
                // wwww bytes that go into the leading surrogate.
                leading = 0xd800 | ((leading - 1) << 6) | ((second_byte & 0xf) << 2) | ((third_byte & 0x3f) >> 4);

                // 0xdc00 is the mask for the trailing surrogate.
                typename To::value_type trailing =
                    0xdc00 | ((third_byte & 0xf) << 6) | ((*begin) & 0x3f);

                dest.push_back(leading);
                dest.push_back(trailing);
            }
            else
            {
                // bad code detected!
                result = false;
                break;
            }

            ++begin;
        }

        return result;
    }
};


//----------------------------------------------------------------------------
//                          UTFConverter Template
//----------------------------------------------------------------------------

/**
 * Specialization that will convert UTF-8 to UTF-32.
 * <p>
 * @author       Jim Park
 * @copyright    2012
 */

template <typename FromIter, typename To>
struct UTFConverter < 1, 4, FromIter, To >
{
    bool operator()(FromIter begin, FromIter end, To& dest)
    {
        bool result = true;

        unsigned char code;
        unsigned char second_byte;
        unsigned char third_byte;

        while (begin != end)
        {
            code = (unsigned char)(*begin);

            if (code <= 0x007f)
            {
                dest.push_back(code);
            }
            else if (code >> 5 == 0x6) // 110 prefix
            {
                // 2-byte sequence.
                if (++begin == end)
                {
                    result = false;
                    break;
                }

                dest.push_back(((code & 0x1f) << 6) | ((*begin) & 0x3f));
            }
            else if (code >> 4 == 0xe) // 1110 prefix
            {
                // 3-byte sequence
                if (++begin == end)
                {
                    result = false;
                    break;
                }
                second_byte = (unsigned char)(*begin);

                if (++begin == end)
                {
                    result = false;
                    break;
                }

                dest.push_back(
                    ((code & 0x0f) << 12) |        // zzzz.... ........
                    ((second_byte & 0x3f) << 6) |  // zzzzyyyy yy......
                    ((*begin) & 0x3f));            // zzzzyyyy yyxxxxxx
            }
            else if (code >> 3 == 0x1e) // 11110 prefix
            {
                // 4-byte sequence
                if (++begin == end)
                {
                    result = false;
                    break;
                }
                second_byte = (unsigned char)(*begin);

                if (++begin == end)
                {
                    result = false;
                    break;
                }
                third_byte = (unsigned char)(*begin);

                if (++begin == end)
                {
                    result = false;
                    break;
                }

                dest.push_back(
                    ((code & 0x07) << 18) |        // 000uuu.. ........ ........
                    ((second_byte & 0x3f) << 12) | // 000uuuuu zzzz.... ........
                    ((third_byte & 0x3f) << 6) |   // 000uuuuu zzzzyyyy yy......
                    ((*begin) & 0x3f));            // 000uuuuu zzzzyyyy yyxxxxxx
            }
            else
            {
                // bad code detected!
                result = false;
                break;
            }

            ++begin;
        }

        return result;
    }
};


//----------------------------------------------------------------------------
//                          UTFConverter Template
//----------------------------------------------------------------------------

/**
 * Specialization that will convert UTF-16 to UTF-8.
 * <p>
 * @author       Jim Park
 * @copyright    2012
 */

template <typename FromIter, typename To>
struct UTFConverter < 2, 1, FromIter, To >
{
    bool operator()(FromIter begin, FromIter end, To& dest)
    {
        bool bNeedSurrogate = false;
        bool result = true;

        unsigned short code;
        unsigned short lead;

        while (begin != end)
        {
            code = (unsigned short)(*begin);

            if (!bNeedSurrogate)
            {
                // Check to see if it's a leading surrgate.
                if ((code >> 10) == 0x36)
                {
                    lead = code;
                    bNeedSurrogate = true;
                }
                else if (code <= 0x7f)
                {
                    // 1-byte character
                    dest.push_back(static_cast<typename To::value_type>(code));
                }
                else if (code <= 0x7ff)
                {
                    // 2-byte character
                    dest.push_back(0xc0 | (code >> 6));   // 110yyyyy
                    dest.push_back(0x80 | (code & 0x3f)); // 10xxxxxx
                }
                else
                {
                    // 3-byte character
                    if (code > 0xd7ff && code < 0xe000)
                    {
                        result = false;
                        break;
                    }
                    else
                    {
                        dest.push_back(0xe0 | (code >> 12));         // 1110zzzz
                        dest.push_back(0x80 | ((code >> 6) & 0x3f)); // 10yyyyyy
                        dest.push_back(0x80 | (code & 0x3f));        // 10xxxxxx
                    }
                }
            }
            else
            {
                if ((code >> 10) == 0x37)
                {
                    // 4-byte character
                    unsigned char uBytes = ((lead >> 6) & 0x0f) + 1;
                    dest.push_back(0xf0 | (uBytes >> 2));                               // 11110uuu
                    dest.push_back(0x80 | (uBytes & 0x03) << 4 | (lead & 0x3f) >> 2);   // 10uuzzzz
                    dest.push_back(0x80 | ((lead & 0x03) << 4) | ((code >> 6) & 0x0f)); // 10yyyyyy
                    dest.push_back(0x80 | (code & 0x3f));                               // 10xxxxxx

                    bNeedSurrogate = false;
                }
                else
                {
                    result = false;
                    break;
                }
            }

            ++begin;
        }

        return result;
    }
};


//----------------------------------------------------------------------------
//                          UTFConverter Template
//----------------------------------------------------------------------------

/**
 * Specialization that will convert UTF-32 to UTF-8.
 * <p>
 * @author       Jim Park
 * @copyright    2012
 */

template <typename FromIter, typename To>
struct UTFConverter < 4, 1, FromIter, To >
{
    bool operator()(FromIter begin, FromIter end, To& dest)
    {
        bool result = true;

        unsigned int code;

        while (begin != end)
        {
            code = (unsigned int)(*begin);

            if (code <= 0x7f)
            {
                // 1-byte character
                dest.push_back(static_cast<typename To::value_type>(code));
            }
            else if (code <= 0x7ff)
            {
                // 2-byte character
                dest.push_back(0xc0 | (code >> 6));   // 110yyyyy
                dest.push_back(0x80 | (code & 0x3f)); // 10xxxxxx
            }
            else if (code <= 0xffff)
            {
                // 3-byte character
                if (code > 0xd7ff && code < 0xe000)
                {
                    result = false;
                    break;
                }
                else
                {
                    dest.push_back(0xe0 | (code >> 12));         // 1110zzzz
                    dest.push_back(0x80 | ((code >> 6) & 0x3f)); // 10yyyyyy
                    dest.push_back(0x80 | (code & 0x3f));        // 10xxxxxx
                }
            }
            else if (code <= 0x1fffff)
            {
                // 4-byte character
                dest.push_back(0xf0 | (code >> 18));                // 11110uuu
                dest.push_back(0x80 | ((code & 0x0003ffff) >> 12)); // 10uuzzzz
                dest.push_back(0x80 | ((code & 0x0fff) >> 6));      // 10yyyyyy
                dest.push_back(0x80 | (code & 0x3f));               // 10xxxxxx
            }
            else
            {
                // Illegal range.
                result = false;
                break;
            }

            ++begin;
        }

        return result;
    }
};


//----------------------------------------------------------------------------
//                          UTFConverter Template
//----------------------------------------------------------------------------

/**
 * Specialization that will convert UTF-16 to UTF-32.
 * <p>
 * @author       Jim Park
 * @copyright    2012
 */

template <typename FromIter, typename To>
struct UTFConverter < 2, 4, FromIter, To >
{
    bool operator()(FromIter begin, FromIter end, To& dest)
    {
        const unsigned int kSurrogateOffset = 0x10000 - (0xd800 << 10) - 0xdc00;
        bool bNeedSurrogate = false;
        bool result = true;

        unsigned short code;
        unsigned short lead;

        while (begin != end)
        {
            code = (unsigned short)(*begin);

            if (!bNeedSurrogate)
            {
                // Check to see if it's a leading surrgate.
                if ((code >> 10) == 0x36)
                {
                    lead = code;
                    bNeedSurrogate = true;
                }
                else
                {
                    if (code > 0xd7ff && code < 0xe000)
                    {
                        result = false;
                        break;
                    }
                    else
                    {
                        dest.push_back(code);
                    }
                }
            }
            else
            {
                if ((code >> 10) == 0x37)
                {
                    dest.push_back((lead << 10) + code + kSurrogateOffset);
                    bNeedSurrogate = false;
                }
                else
                {
                    result = false;
                    break;
                }
            }

            ++begin;
        }

        return result;
    }
};


//----------------------------------------------------------------------------
//                          UTFConverter Template
//----------------------------------------------------------------------------

/**
 * Specialization that will convert UTF-32 to UTF-16.
 * <p>
 * @author       Jim Park
 * @copyright    2012
 */

template <typename FromIter, typename To>
struct UTFConverter < 4, 2, FromIter, To >
{
    bool operator()(FromIter begin, FromIter end, To& dest)
    {
        const unsigned int kLeadOffset = 0xd800 - (0x10000 >> 10);

        bool result = true;
        unsigned int code;

        while (begin != end)
        {
            code = (unsigned int)(*begin);
            if (code < 0x10000)
            {
                if (code > 0xd7ff && code < 0xe000)
                {
                    result = false;
                    break;
                }
                dest.push_back(static_cast<typename To::value_type>(code));
            }
            else
            {
                if (code > 0x10ffff)
                {
                    result = false;
                    break;
                }
                dest.push_back(static_cast<typename To::value_type>(kLeadOffset + (code >> 10)));
                dest.push_back(static_cast<typename To::value_type>(0xdc00 | (code & 0x3ff)));
            }

            ++begin;
        }

        return result;
    }
};


//----------------------------------------------------------------------------
//                          UTFConverter Template
//----------------------------------------------------------------------------

/**
 * Specialization that will simply copy the characters over if the encoding
 * is actually the same. This will also be used if for example the original
 * string was of unsigned short and the resulting string is of char16_t.
 * <p>
 * @author       Jim Park
 * @copyright    2012
 */

template <int UCS, typename FromIter, typename To>
struct UTFConverter < UCS, UCS, FromIter, To >
{
    bool operator()(FromIter begin, FromIter end, To& dest)
    {
        dest.append(begin, end);
        return true;
    }
};

//----------------------------------------------------------------------------
//                       StringTypeResolver Template
//----------------------------------------------------------------------------

/**
 * The StringTypeResolver template is used to resolve how to get the begin and
 * end iterator depending on the string type, either based on a class that has
 * begin() and end() or a pointer to a character class that needs to compute
 * the begin and end pointers.
 * <p>
 * @author       Jim Park
 * @copyright    2012
 */
template <typename T>
struct StringTypeResolver
{
    typedef typename T::value_type value_type;
    typedef typename T::const_iterator const_iterator;
    enum { Size = sizeof(value_type) };

    static const_iterator begin(const T& str)
    {
        return str.begin();
    }

    static const_iterator end(const T& str)
    {
        return str.end();
    }
};

//----------------------------------------------------------------------------
//                     StringIteratorResolver Template
//----------------------------------------------------------------------------

/**
 * The StringTypeResolver template specialization for strings that are
 * really pointers to character types.
 * <p>
 * @author       Jim Park
 * @copyright    2012
 */
template <typename CharType>
struct StringTypeResolver < CharType* >
{
    typedef CharType value_type;
    typedef const CharType* const_iterator;
    enum { Size = sizeof(value_type) };

    static const_iterator begin(const CharType* str)
    {
        return str;
    }

    static const_iterator end(const CharType* str)
    {
        while (*str != 0) ++str;
        return str;
    }
};

//----------------------------------------------------------------------------
//                     StringIterTypeResolver Template
//----------------------------------------------------------------------------

/**
 * The StringIterTypeResolver template is used to resolve the type of the
 * iterator, whether it really is an iterator from a string class or a pointer
 * to a character class.
 * <p>
 * @author       Jim Park
 * @copyright    2012
 */

template <typename T>
struct StringIterTypeResolver
{
    typedef T iterator;
    typedef typename T::value_type value_type;
    enum { Size = sizeof(value_type) };
};

//----------------------------------------------------------------------------
//                     StringIterTypeResolver Template
//----------------------------------------------------------------------------

/**
 * The StringIterTypeResolver template specialization for strings that are
 * really pointers to character types.
 * <p>
 * @author       Jim Park
 * @copyright    2012
 */

template <typename CharType>
struct StringIterTypeResolver < CharType* >
{
    typedef CharType* iterator;
    typedef CharType value_type;
    enum { Size = sizeof(value_type) };
};

template <int>
struct WTUTIL_DSPEC UTFErrorStrings
{
    // Nothing defined on purpose to create compiler errors for badly specified
    // UTFErrorStrings specialization.
};

template <>
struct WTUTIL_DSPEC UTFErrorStrings < 1 >
{
    static const TCHAR* s_strBadlyFormed;
};

template <>
struct WTUTIL_DSPEC UTFErrorStrings < 2 >
{
    static const TCHAR* s_strBadlyFormed;
};

template <>
struct WTUTIL_DSPEC UTFErrorStrings < 4 >
{
    static const TCHAR* s_strBadlyFormed;
};

} // namespace detail

//----------------------------------------------------------------------------
// FormatException
//----------------------------------------------------------------------------

/**
 * This exception is thrown if the source string to convert into the various
 * Unicode formats is not a valid Unicode string.
 * @author       Jim Park
 * @copyright    2012
 */ /
class FormatException
{
public:
    FormatException(char* msg, char* file, int line)
        : msg_(msg), file_(file), line_(line)
    {
    }

    virtual ~FormatException() {}

    virtual const char* msg() const
    {
        return msg_.c_str();
    }

    virtual const char* file() const
    {
        return file_.c_str();
    }

    virtual int line() const
    {
        return line_;
    }

private:
    string msg_;
    string file_;
    int    line_;
};

//----------------------------------------------------------------------------
// UTFConvert
//----------------------------------------------------------------------------

/**
 * This template function will resolve string to string conversions. It will
 * create a new string with the desired UTF encoding from the 'from' string.
 * <p>
 * <b>Example:</b>
 * <pre>
 *    const wstring from(WT_U16("Hello, world!"));
 *    tstring result(UTFConvert< tstring >(from));
 * </pre>
 * <p>
 * <b>Note:</b> This function assumes that the byte-order (endianness) of the
 * original string and the returned string is the native byte-order for the
 * platform.
 *
 * @param from The original string to convert.
 * @return A new string with the desired UTF encoding.
 * @exception CFormatException if the UTF encoding of the original string is
 *    invalid.<br>
 *    <b>Note:</b> This exception is not thrown for certain strings that do
 *    not conform to UTF encoding. For reliability, if you need to determine
 *    if a string is valid, it is recommended that you use
 *    <code>CValidateUnicode::ValidateUTF8()</code> or
 *    <code>CValidateUnicode::ValidateUTF16()</code> before calling
 *    this function.
 * @author       Jim Park
 * @copyright    2012
 */
template <typename ToStrType, typename FromStrType>
ToStrType UTFConvert(const FromStrType& from)
{
    typedef detail::StringTypeResolver<FromStrType> FromType;

    detail::UTFConverter < FromType::Size,
        sizeof(typename ToStrType::value_type),
        typename FromType::const_iterator,
        ToStrType > converter;

    ToStrType result;
    if (!converter(FromType::begin(from), FromType::end(from), result))
    {
        throw FormatException(
            detail::UTFErrorStrings<FromType::Size>::s_strBadlyFormed,
            __FILE__, __LINE__);
    }

    return result;
}

//----------------------------------------------------------------------------
// UTFConvert
//----------------------------------------------------------------------------

/**
 * Convert the characters in the specified range to a new string with
 * the UTF encoding that corresponds to the size of characters (elements) of
 * the specified string type.
 * <p>
 * In addition, the characters in the source container are expected to
 * conform to the UTF encoding that corresponds to the size of
 * the characters (elements) in the source container.
 * <p>
 * <b>Example:</b>
 * <pre>
 *    const wstring from(WT_U16("Hello, world!"));
 *    tstring result(
 *       UTFConvert< tstring >(
 *          from.begin(), from.end()));
 * </pre>
 * <p>
 * <b>Note:</b> This function assumes that the byte-order (endianness) of
 * the original string and the returned string is the native byte-order for
 * the platform.
 *
 * @param begin the iterator to the beginning (inclusive) of the range to
 *    convert.
 * @param end the iterator to the end (exclusive) of the range to convert.
 * @return a new string with the specified UTF encoding.
 * @exception CFormatException if the UTF encoding of the original string is
 *    invalid.<br>
 *    <b>Note:</b> This exception is not thrown for certain strings that do
 *    not conform to UTF encoding. For reliability, if you need to determine
 *    if a string is valid, it is recommended that you use
 *    <code>CValidateUnicode::ValidateUTF8()</code> or
 *    <code>CValidateUnicode::ValidateUTF16()</code> before calling
 *    this function.
 * @author       Jim Park
 * @copyright    2012
 */
template <typename ToStrType, typename FromIterType>
ToStrType UTFConvert(
    FromIterType begin,
    FromIterType end)
{
    typedef detail::StringIterTypeResolver<FromIterType> FromType;

    detail::UTFConverter < FromType::Size,
        sizeof(typename ToStrType::value_type),
        typename FromType::iterator,
        ToStrType > converter;

    ToStrType result;
    if (!converter(begin, end, result))
    {
        throw FormatException(
            detail::UTFErrorStrings<FromType::Size>::s_strBadlyFormed,
            __FILE__, __LINE__);
    }

    return result;
}

//----------------------------------------------------------------------------
// UTFConvert
//----------------------------------------------------------------------------

/**
 * This template function will resolve iterators to string conversions. It
 * will append to the destination string the characters in the range from
 * [begin, end).
 * <p>
 * For example:
 * <pre>
 *    wstring from(WT_U16("Hello, world!"));
 *    tstring result;
 *    UTFConvert(from.begin(), from.end(), result);
 * </pre>
 * <p>
 * <b>Note:</b> This function assumes that the byte-order (endianness) of the
 * original string and the returned string is the native byte-order for the
 * platform.
 * <p>
 * @param begin The iterator (or pointer) to the beginning of the range to
 *    convert. (inclusive).
 * @param end The iterator (or pointer) to the end of the range to convert.
 *    (non-inclusive).
 * @param dest [out] The destination string to append to.
 * @return True, if successful, false otherwise.
 * @author       Jim Park
 * @copyright    2012
 */

template <typename ToStrType, typename FromIterType>
bool UTFConvert(
    FromIterType begin,
    FromIterType end,
    ToStrType&   dest)
{
    typedef detail::StringIterTypeResolver<FromIterType> FromType;

    detail::UTFConverter < FromType::Size,
        sizeof(typename ToStrType::value_type),
        typename FromType::iterator,
        ToStrType > converter;

    return converter(begin, end, dest);
}

#endif  // WT_WTUTIL_UTFCONVERT_

