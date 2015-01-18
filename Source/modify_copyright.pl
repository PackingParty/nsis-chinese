#!perl -w

use FileHandle;
use strict;

if (@ARGV != 2)
{
    print "Usage: $0 <previous year> <new year>\n";
}

my @filetypes = ("*.cpp", "*.c", "*.h", "*.hpp");
my @files;
my $file;
my $prev_year = shift @ARGV;
my $new_year = shift @ARGV;


foreach $file (@filetypes)
{
    push @files, (glob $file);
}

foreach $file (@files)
{
    my @buf;
    my $fp = new FileHandle($file);
    
    while (<$fp>)
    {
        if (/^\s*\*.*Copyright/)
        {
            s/\b$prev_year\b/$new_year/;
        }
        push @buf, $_;
    }

    $fp->close();

    $fp->open(">$file") or die;
    print $fp @buf;
    $fp->close();
}
