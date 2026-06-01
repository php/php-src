--TEST--
Test mb_regex_encoding() function : usage variations - test different encodings
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_regex_encoding') or die("skip mb_regex_encoding() is not available in this build");
?>
--FILE--
<?php
/*
 * Test all listed encoding types from php.net to check all are known to function
 * NB: The strings passed are *NOT* necessarily encoded in the encoding passed to the function.
 * This test is purely to see whether the function recognises the encoding.
 */

echo "*** Testing mb_regex_encoding() : usage variations ***\n";

$encoding = array('UCS-4',			/*1*/
                  'UCS-4BE',
                  'UCS-4LE',
                  'UCS-2',
                  'UCS-2BE',		/*5*/
                  'UCS-2LE',
                  'UTF-32',
                  'UTF-32BE',
                  'UTF-32LE',
                  'UTF-16',			/*10*/
                  'UTF-16BE',
                  'UTF-16LE',
                  'UTF-7',
                  'UTF7-IMAP',
                  'UTF-8',			/*15*/
                  'ASCII',
                  'EUC-JP',
                  'SJIS',
                  'eucJP-win',
                  'SJIS-win',		/*20*/
                  'ISO-2022-JP',
                  'JIS',
                  'ISO-8859-1',
                  'ISO-8859-2',
                  'ISO-8859-3',		/*25*/
                  'ISO-8859-4',
                  'ISO-8859-5',
                  'ISO-8859-6',
                  'ISO-8859-7',
                  'ISO-8859-8',		/*30*/
                  'ISO-8859-9',
                  'ISO-8859-10',
                  'ISO-8859-13',
                  'ISO-8859-14',
                  'ISO-8859-15',	/*35*/
                  'byte2be',
                  'byte2le',
                  'byte4be',
                  'byte4le',
                  'BASE64',			/*40*/
                  'HTML-ENTITIES',
                  '7bit',
                  '8bit',
                  'EUC-CN',
                  'CP936',			/*45*/
                  'HZ',
                  'EUC-TW',
                  'CP950',
                  'BIG-5',
                  'EUC-KR',			/*50*/
                  'UHC',
                  'ISO-2022-KR',
                  'Windows-1251',
                  'Windows-1252',
                  'CP866',			/*55*/
                  'KOI8-R');		/*56*/

$iterator = 1;
foreach($encoding as $enc) {
    echo "\n-- Iteration $iterator --\n";
    var_dump(mb_regex_encoding());
    try {
        var_dump(mb_regex_encoding($enc));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    var_dump(mb_regex_encoding());
    $iterator++;
}
echo "Done";
?>
--EXPECTF--
*** Testing mb_regex_encoding() : usage variations ***

-- Iteration 1 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "UTF-8"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "UCS-4"

-- Iteration 2 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "UCS-4"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "UCS-4BE" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "UCS-4"

-- Iteration 3 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "UCS-4"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(7) "UCS-4LE"

-- Iteration 4 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(7) "UCS-4LE"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "UCS-2" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(7) "UCS-4LE"

-- Iteration 5 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(7) "UCS-4LE"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "UCS-2BE" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(7) "UCS-4LE"

-- Iteration 6 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(7) "UCS-4LE"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "UCS-2LE" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(7) "UCS-4LE"

-- Iteration 7 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(7) "UCS-4LE"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "UCS-4"

-- Iteration 8 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "UCS-4"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "UCS-4"

-- Iteration 9 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "UCS-4"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(7) "UCS-4LE"

-- Iteration 10 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(7) "UCS-4LE"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "UTF-16"

-- Iteration 11 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "UTF-16"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "UTF-16"

-- Iteration 12 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "UTF-16"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(8) "UTF-16LE"

-- Iteration 13 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(8) "UTF-16LE"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "UTF-7" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(8) "UTF-16LE"

-- Iteration 14 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(8) "UTF-16LE"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "UTF7-IMAP" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(8) "UTF-16LE"

-- Iteration 15 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(8) "UTF-16LE"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "UTF-8"

-- Iteration 16 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "UTF-8"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "ASCII"

-- Iteration 17 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "ASCII"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-JP"

-- Iteration 18 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-JP"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(4) "SJIS"

-- Iteration 19 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(4) "SJIS"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-JP"

-- Iteration 20 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-JP"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(4) "SJIS"

-- Iteration 21 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(4) "SJIS"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "ISO-2022-JP" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(4) "SJIS"

-- Iteration 22 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(4) "SJIS"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "JIS" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(4) "SJIS"

-- Iteration 23 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(4) "SJIS"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-1"

-- Iteration 24 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-1"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-2"

-- Iteration 25 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-2"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-3"

-- Iteration 26 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-3"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-4"

-- Iteration 27 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-4"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-5"

-- Iteration 28 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-5"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-6"

-- Iteration 29 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-6"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-7"

-- Iteration 30 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-7"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-8"

-- Iteration 31 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-8"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-9"

-- Iteration 32 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "ISO-8859-9"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-10"

-- Iteration 33 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-10"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-13"

-- Iteration 34 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-13"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-14"

-- Iteration 35 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-14"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

-- Iteration 36 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "byte2be" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

-- Iteration 37 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "byte2le" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

-- Iteration 38 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "byte4be" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

-- Iteration 39 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "byte4le" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

-- Iteration 40 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "BASE64" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

-- Iteration 41 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "HTML-ENTITIES" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

-- Iteration 42 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "7bit" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

-- Iteration 43 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "8bit" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

-- Iteration 44 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(11) "ISO-8859-15"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-CN"

-- Iteration 45 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-CN"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "CP936" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-CN"

-- Iteration 46 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-CN"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "HZ" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-CN"

-- Iteration 47 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-CN"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-TW"

-- Iteration 48 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-TW"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "CP950" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-TW"

-- Iteration 49 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-TW"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(4) "BIG5"

-- Iteration 50 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(4) "BIG5"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-KR"

-- Iteration 51 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-KR"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "UHC" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-KR"

-- Iteration 52 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-KR"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "ISO-2022-KR" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-KR"

-- Iteration 53 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-KR"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "Windows-1251" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-KR"

-- Iteration 54 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-KR"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "Windows-1252" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-KR"

-- Iteration 55 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-KR"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_regex_encoding(): Argument #1 ($encoding) must be a valid encoding, "CP866" given

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-KR"

-- Iteration 56 --

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(6) "EUC-KR"

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(5) "KOI8R"
Done
