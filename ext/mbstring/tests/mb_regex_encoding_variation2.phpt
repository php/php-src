--TEST--
Test mb_regex_encoding() function : usage variations - test different encodings
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_regex_encoding') or die("skip mb_regex_encoding() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_regex_encoding([string $encoding])
 * Description: Returns the current encoding for regex as a string.
 * Source code: ext/mbstring/php_mbregex.c
 */

/*
 * Test all listed encoding types from php.net to check all are known to function
 * NB: The strings passed are *NOT* necessarily encoded in the encoding passed to the function.
 * This test is purely to see whether the function recognises the encoding.
 */

echo "*** Testing mb_regex_encoding() : usage variations ***\n";

$encodings = [
    'UCS-4',		/*1*/
    'UCS-4BE',
    'UCS-4LE',
    'UCS-2',
    'UCS-2BE',		/*5*/
    'UCS-2LE',
    'UTF-32',
    'UTF-32BE',
    'UTF-32LE',
    'UTF-16',		/*10*/
    'UTF-16BE',
    'UTF-16LE',
    'UTF-7',
    'UTF7-IMAP',
    'UTF-8',		/*15*/
    'ASCII',
    'EUC-JP',
    'SJIS',
    'eucJP-win',
    'SJIS-win',		/*20*/
    'ISO-2022-JP',
    'JIS',
    'ISO-8859-1',
    'ISO-8859-2',
    'ISO-8859-3',	/*25*/
    'ISO-8859-4',
    'ISO-8859-5',
    'ISO-8859-6',
    'ISO-8859-7',
    'ISO-8859-8',	/*30*/
    'ISO-8859-9',
    'ISO-8859-10',
    'ISO-8859-13',
    'ISO-8859-14',
    'ISO-8859-15',	/*35*/
    'byte2be',
    'byte2le',
    'byte4be',
    'byte4le',
    'BASE64',		/*40*/
    'HTML-ENTITIES',
    '7bit',
    '8bit',
    'EUC-CN',
    'CP936',		/*45*/
    'HZ',
    'EUC-TW',
    'CP950',
    'BIG-5',
    'EUC-KR',		/*50*/
    'UHC',
    'ISO-2022-KR',
    'Windows-1251',
    'Windows-1252',
    'CP866',		/*55*/
    'KOI8-R',		/*56*/
];

$iterator = 1;
foreach($encodings as $encoding) {
	echo "\n-- Iteration $iterator --\n";
	var_dump(mb_regex_encoding());

    try {
	    var_dump(mb_regex_encoding($encoding));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }

	var_dump(mb_regex_encoding());
	$iterator++;
}

?>
--EXPECT--
*** Testing mb_regex_encoding() : usage variations ***

-- Iteration 1 --
string(5) "UTF-8"
bool(true)
string(5) "UCS-4"

-- Iteration 2 --
string(5) "UCS-4"
Unknown encoding "UCS-4BE"
string(5) "UCS-4"

-- Iteration 3 --
string(5) "UCS-4"
bool(true)
string(7) "UCS-4LE"

-- Iteration 4 --
string(7) "UCS-4LE"
Unknown encoding "UCS-2"
string(7) "UCS-4LE"

-- Iteration 5 --
string(7) "UCS-4LE"
Unknown encoding "UCS-2BE"
string(7) "UCS-4LE"

-- Iteration 6 --
string(7) "UCS-4LE"
Unknown encoding "UCS-2LE"
string(7) "UCS-4LE"

-- Iteration 7 --
string(7) "UCS-4LE"
bool(true)
string(5) "UCS-4"

-- Iteration 8 --
string(5) "UCS-4"
bool(true)
string(5) "UCS-4"

-- Iteration 9 --
string(5) "UCS-4"
bool(true)
string(7) "UCS-4LE"

-- Iteration 10 --
string(7) "UCS-4LE"
bool(true)
string(6) "UTF-16"

-- Iteration 11 --
string(6) "UTF-16"
bool(true)
string(6) "UTF-16"

-- Iteration 12 --
string(6) "UTF-16"
bool(true)
string(8) "UTF-16LE"

-- Iteration 13 --
string(8) "UTF-16LE"
Unknown encoding "UTF-7"
string(8) "UTF-16LE"

-- Iteration 14 --
string(8) "UTF-16LE"
Unknown encoding "UTF7-IMAP"
string(8) "UTF-16LE"

-- Iteration 15 --
string(8) "UTF-16LE"
bool(true)
string(5) "UTF-8"

-- Iteration 16 --
string(5) "UTF-8"
bool(true)
string(5) "ASCII"

-- Iteration 17 --
string(5) "ASCII"
bool(true)
string(6) "EUC-JP"

-- Iteration 18 --
string(6) "EUC-JP"
bool(true)
string(4) "SJIS"

-- Iteration 19 --
string(4) "SJIS"
bool(true)
string(6) "EUC-JP"

-- Iteration 20 --
string(6) "EUC-JP"
bool(true)
string(4) "SJIS"

-- Iteration 21 --
string(4) "SJIS"
Unknown encoding "ISO-2022-JP"
string(4) "SJIS"

-- Iteration 22 --
string(4) "SJIS"
Unknown encoding "JIS"
string(4) "SJIS"

-- Iteration 23 --
string(4) "SJIS"
bool(true)
string(10) "ISO-8859-1"

-- Iteration 24 --
string(10) "ISO-8859-1"
bool(true)
string(10) "ISO-8859-2"

-- Iteration 25 --
string(10) "ISO-8859-2"
bool(true)
string(10) "ISO-8859-3"

-- Iteration 26 --
string(10) "ISO-8859-3"
bool(true)
string(10) "ISO-8859-4"

-- Iteration 27 --
string(10) "ISO-8859-4"
bool(true)
string(10) "ISO-8859-5"

-- Iteration 28 --
string(10) "ISO-8859-5"
bool(true)
string(10) "ISO-8859-6"

-- Iteration 29 --
string(10) "ISO-8859-6"
bool(true)
string(10) "ISO-8859-7"

-- Iteration 30 --
string(10) "ISO-8859-7"
bool(true)
string(10) "ISO-8859-8"

-- Iteration 31 --
string(10) "ISO-8859-8"
bool(true)
string(10) "ISO-8859-9"

-- Iteration 32 --
string(10) "ISO-8859-9"
bool(true)
string(11) "ISO-8859-10"

-- Iteration 33 --
string(11) "ISO-8859-10"
bool(true)
string(11) "ISO-8859-13"

-- Iteration 34 --
string(11) "ISO-8859-13"
bool(true)
string(11) "ISO-8859-14"

-- Iteration 35 --
string(11) "ISO-8859-14"
bool(true)
string(11) "ISO-8859-15"

-- Iteration 36 --
string(11) "ISO-8859-15"
Unknown encoding "byte2be"
string(11) "ISO-8859-15"

-- Iteration 37 --
string(11) "ISO-8859-15"
Unknown encoding "byte2le"
string(11) "ISO-8859-15"

-- Iteration 38 --
string(11) "ISO-8859-15"
Unknown encoding "byte4be"
string(11) "ISO-8859-15"

-- Iteration 39 --
string(11) "ISO-8859-15"
Unknown encoding "byte4le"
string(11) "ISO-8859-15"

-- Iteration 40 --
string(11) "ISO-8859-15"
Unknown encoding "BASE64"
string(11) "ISO-8859-15"

-- Iteration 41 --
string(11) "ISO-8859-15"
Unknown encoding "HTML-ENTITIES"
string(11) "ISO-8859-15"

-- Iteration 42 --
string(11) "ISO-8859-15"
Unknown encoding "7bit"
string(11) "ISO-8859-15"

-- Iteration 43 --
string(11) "ISO-8859-15"
Unknown encoding "8bit"
string(11) "ISO-8859-15"

-- Iteration 44 --
string(11) "ISO-8859-15"
bool(true)
string(6) "EUC-CN"

-- Iteration 45 --
string(6) "EUC-CN"
Unknown encoding "CP936"
string(6) "EUC-CN"

-- Iteration 46 --
string(6) "EUC-CN"
Unknown encoding "HZ"
string(6) "EUC-CN"

-- Iteration 47 --
string(6) "EUC-CN"
bool(true)
string(6) "EUC-TW"

-- Iteration 48 --
string(6) "EUC-TW"
Unknown encoding "CP950"
string(6) "EUC-TW"

-- Iteration 49 --
string(6) "EUC-TW"
bool(true)
string(4) "BIG5"

-- Iteration 50 --
string(4) "BIG5"
bool(true)
string(6) "EUC-KR"

-- Iteration 51 --
string(6) "EUC-KR"
Unknown encoding "UHC"
string(6) "EUC-KR"

-- Iteration 52 --
string(6) "EUC-KR"
Unknown encoding "ISO-2022-KR"
string(6) "EUC-KR"

-- Iteration 53 --
string(6) "EUC-KR"
Unknown encoding "Windows-1251"
string(6) "EUC-KR"

-- Iteration 54 --
string(6) "EUC-KR"
Unknown encoding "Windows-1252"
string(6) "EUC-KR"

-- Iteration 55 --
string(6) "EUC-KR"
Unknown encoding "CP866"
string(6) "EUC-KR"

-- Iteration 56 --
string(6) "EUC-KR"
bool(true)
string(5) "KOI8R"
