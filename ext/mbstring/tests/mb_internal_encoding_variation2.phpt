--TEST--
Test mb_internal_encoding() function : usage variations - Test different encodings
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_internal_encoding') or die("skip mb_internal_encoding() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_internal_encoding([string $encoding])
 * Description: Sets the current internal encoding or Returns 
 * the current internal encoding as a string 
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Test all listed encoding types from php.net to check all are known to function
 * NB: The strings passed are *NOT* necessarily encoded in the encoding passed to the function.
 * This test is purely to see whether the function recognises the encoding.
 */

echo "*** Testing mb_internal_encoding() : usage variations ***\n";

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
	var_dump(mb_internal_encoding());
	var_dump(mb_internal_encoding($enc));
	var_dump(mb_internal_encoding());
	$iterator++;
}
echo "Done";
?>
--EXPECTF--

*** Testing mb_internal_encoding() : usage variations ***

-- Iteration 1 --
string(%d) "%s"
bool(true)
string(5) "UCS-4"

-- Iteration 2 --
string(5) "UCS-4"
bool(true)
string(7) "UCS-4BE"

-- Iteration 3 --
string(7) "UCS-4BE"
bool(true)
string(7) "UCS-4LE"

-- Iteration 4 --
string(7) "UCS-4LE"
bool(true)
string(5) "UCS-2"

-- Iteration 5 --
string(5) "UCS-2"
bool(true)
string(7) "UCS-2BE"

-- Iteration 6 --
string(7) "UCS-2BE"
bool(true)
string(7) "UCS-2LE"

-- Iteration 7 --
string(7) "UCS-2LE"
bool(true)
string(6) "UTF-32"

-- Iteration 8 --
string(6) "UTF-32"
bool(true)
string(8) "UTF-32BE"

-- Iteration 9 --
string(8) "UTF-32BE"
bool(true)
string(8) "UTF-32LE"

-- Iteration 10 --
string(8) "UTF-32LE"
bool(true)
string(6) "UTF-16"

-- Iteration 11 --
string(6) "UTF-16"
bool(true)
string(8) "UTF-16BE"

-- Iteration 12 --
string(8) "UTF-16BE"
bool(true)
string(8) "UTF-16LE"

-- Iteration 13 --
string(8) "UTF-16LE"
bool(true)
string(5) "UTF-7"

-- Iteration 14 --
string(5) "UTF-7"
bool(true)
string(9) "UTF7-IMAP"

-- Iteration 15 --
string(9) "UTF7-IMAP"
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
string(9) "eucJP-win"

-- Iteration 20 --
string(9) "eucJP-win"
bool(true)
string(8) "SJIS-win"

-- Iteration 21 --
string(8) "SJIS-win"
bool(true)
string(11) "ISO-2022-JP"

-- Iteration 22 --
string(11) "ISO-2022-JP"
bool(true)
string(3) "JIS"

-- Iteration 23 --
string(3) "JIS"
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
bool(true)
string(7) "byte2be"

-- Iteration 37 --
string(7) "byte2be"
bool(true)
string(7) "byte2le"

-- Iteration 38 --
string(7) "byte2le"
bool(true)
string(7) "byte4be"

-- Iteration 39 --
string(7) "byte4be"
bool(true)
string(7) "byte4le"

-- Iteration 40 --
string(7) "byte4le"
bool(true)
string(6) "BASE64"

-- Iteration 41 --
string(6) "BASE64"
bool(true)
string(13) "HTML-ENTITIES"

-- Iteration 42 --
string(13) "HTML-ENTITIES"
bool(true)
string(4) "7bit"

-- Iteration 43 --
string(4) "7bit"
bool(true)
string(4) "8bit"

-- Iteration 44 --
string(4) "8bit"
bool(true)
string(6) "EUC-CN"

-- Iteration 45 --
string(6) "EUC-CN"
bool(true)
string(5) "CP936"

-- Iteration 46 --
string(5) "CP936"
bool(true)
string(2) "HZ"

-- Iteration 47 --
string(2) "HZ"
bool(true)
string(6) "EUC-TW"

-- Iteration 48 --
string(6) "EUC-TW"
bool(true)
string(5) "BIG-5"

-- Iteration 49 --
string(5) "BIG-5"
bool(true)
string(5) "BIG-5"

-- Iteration 50 --
string(5) "BIG-5"
bool(true)
string(6) "EUC-KR"

-- Iteration 51 --
string(6) "EUC-KR"
bool(true)
string(3) "UHC"

-- Iteration 52 --
string(3) "UHC"
bool(true)
string(11) "ISO-2022-KR"

-- Iteration 53 --
string(11) "ISO-2022-KR"
bool(true)
string(12) "Windows-1251"

-- Iteration 54 --
string(12) "Windows-1251"
bool(true)
string(12) "Windows-1252"

-- Iteration 55 --
string(12) "Windows-1252"
bool(true)
string(5) "CP866"

-- Iteration 56 --
string(5) "CP866"
bool(true)
string(6) "KOI8-R"
Done