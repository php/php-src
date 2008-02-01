--TEST--
Test mb_substr() function : usage variations - test different encodings
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_substr') or die("skip mb_substr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_substr(string $str, int $start [, int $length [, string $encoding]])
 * Description: Returns part of a string
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass all encodings listed on php.net to test that function recognises them.
 * NB: The strings passed are *NOT* necessarily encoded in the encoding passed to the function.
 * This test is purely to see whether the function recognises the encoding.
 */

echo "*** Testing mb_substr() : usage variations ***\n";

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
$string_ascii = 'abc def';
//Japanese string encoded in UTF-8
$string_mb = base64_decode('44K/44OT44Ol44Os44O844OG44Kj44Oz44Kw44O744Oe44K344O844Oz44O744Kr44Oz44OR44OL44O8');

foreach($encoding as $enc) {
	echo "\n-- Iteration $iterator: $enc --\n";

	echo "-- ASCII String --\n";
	if (mb_substr($string_ascii, 1, 5, $enc)) {
		echo "Encoding $enc recognised\n";
	} else {
		echo "Encoding $enc not recognised\n";
	}

	echo "-- Multibyte String --\n";
	if (mb_substr($string_mb, 1, 5, $enc)) {
		echo "Encoding $enc recognised\n";
	} else {
		echo "Encoding $enc not recognised\n";
	}
}

echo "Done";
?>
--EXPECTF--
*** Testing mb_substr() : usage variations ***

-- Iteration 1: UCS-4 --
-- ASCII String --
Encoding UCS-4 recognised
-- Multibyte String --
Encoding UCS-4 recognised

-- Iteration 1: UCS-4BE --
-- ASCII String --
Encoding UCS-4BE recognised
-- Multibyte String --
Encoding UCS-4BE recognised

-- Iteration 1: UCS-4LE --
-- ASCII String --
Encoding UCS-4LE recognised
-- Multibyte String --
Encoding UCS-4LE recognised

-- Iteration 1: UCS-2 --
-- ASCII String --
Encoding UCS-2 recognised
-- Multibyte String --
Encoding UCS-2 recognised

-- Iteration 1: UCS-2BE --
-- ASCII String --
Encoding UCS-2BE recognised
-- Multibyte String --
Encoding UCS-2BE recognised

-- Iteration 1: UCS-2LE --
-- ASCII String --
Encoding UCS-2LE recognised
-- Multibyte String --
Encoding UCS-2LE recognised

-- Iteration 1: UTF-32 --
-- ASCII String --
Encoding UTF-32 recognised
-- Multibyte String --
Encoding UTF-32 recognised

-- Iteration 1: UTF-32BE --
-- ASCII String --
Encoding UTF-32BE recognised
-- Multibyte String --
Encoding UTF-32BE recognised

-- Iteration 1: UTF-32LE --
-- ASCII String --
Encoding UTF-32LE recognised
-- Multibyte String --
Encoding UTF-32LE recognised

-- Iteration 1: UTF-16 --
-- ASCII String --
Encoding UTF-16 recognised
-- Multibyte String --
Encoding UTF-16 recognised

-- Iteration 1: UTF-16BE --
-- ASCII String --
Encoding UTF-16BE recognised
-- Multibyte String --
Encoding UTF-16BE recognised

-- Iteration 1: UTF-16LE --
-- ASCII String --
Encoding UTF-16LE recognised
-- Multibyte String --
Encoding UTF-16LE recognised

-- Iteration 1: UTF-7 --
-- ASCII String --
Encoding UTF-7 recognised
-- Multibyte String --
Encoding UTF-7 recognised

-- Iteration 1: UTF7-IMAP --
-- ASCII String --
Encoding UTF7-IMAP recognised
-- Multibyte String --
Encoding UTF7-IMAP recognised

-- Iteration 1: UTF-8 --
-- ASCII String --
Encoding UTF-8 recognised
-- Multibyte String --
Encoding UTF-8 recognised

-- Iteration 1: ASCII --
-- ASCII String --
Encoding ASCII recognised
-- Multibyte String --
Encoding ASCII recognised

-- Iteration 1: EUC-JP --
-- ASCII String --
Encoding EUC-JP recognised
-- Multibyte String --
Encoding EUC-JP recognised

-- Iteration 1: SJIS --
-- ASCII String --
Encoding SJIS recognised
-- Multibyte String --
Encoding SJIS recognised

-- Iteration 1: eucJP-win --
-- ASCII String --
Encoding eucJP-win recognised
-- Multibyte String --
Encoding eucJP-win recognised

-- Iteration 1: SJIS-win --
-- ASCII String --
Encoding SJIS-win recognised
-- Multibyte String --
Encoding SJIS-win recognised

-- Iteration 1: ISO-2022-JP --
-- ASCII String --
Encoding ISO-2022-JP recognised
-- Multibyte String --
Encoding ISO-2022-JP recognised

-- Iteration 1: JIS --
-- ASCII String --
Encoding JIS recognised
-- Multibyte String --
Encoding JIS recognised

-- Iteration 1: ISO-8859-1 --
-- ASCII String --
Encoding ISO-8859-1 recognised
-- Multibyte String --
Encoding ISO-8859-1 recognised

-- Iteration 1: ISO-8859-2 --
-- ASCII String --
Encoding ISO-8859-2 recognised
-- Multibyte String --
Encoding ISO-8859-2 recognised

-- Iteration 1: ISO-8859-3 --
-- ASCII String --
Encoding ISO-8859-3 recognised
-- Multibyte String --
Encoding ISO-8859-3 recognised

-- Iteration 1: ISO-8859-4 --
-- ASCII String --
Encoding ISO-8859-4 recognised
-- Multibyte String --
Encoding ISO-8859-4 recognised

-- Iteration 1: ISO-8859-5 --
-- ASCII String --
Encoding ISO-8859-5 recognised
-- Multibyte String --
Encoding ISO-8859-5 recognised

-- Iteration 1: ISO-8859-6 --
-- ASCII String --
Encoding ISO-8859-6 recognised
-- Multibyte String --
Encoding ISO-8859-6 recognised

-- Iteration 1: ISO-8859-7 --
-- ASCII String --
Encoding ISO-8859-7 recognised
-- Multibyte String --
Encoding ISO-8859-7 recognised

-- Iteration 1: ISO-8859-8 --
-- ASCII String --
Encoding ISO-8859-8 recognised
-- Multibyte String --
Encoding ISO-8859-8 recognised

-- Iteration 1: ISO-8859-9 --
-- ASCII String --
Encoding ISO-8859-9 recognised
-- Multibyte String --
Encoding ISO-8859-9 recognised

-- Iteration 1: ISO-8859-10 --
-- ASCII String --
Encoding ISO-8859-10 recognised
-- Multibyte String --
Encoding ISO-8859-10 recognised

-- Iteration 1: ISO-8859-13 --
-- ASCII String --
Encoding ISO-8859-13 recognised
-- Multibyte String --
Encoding ISO-8859-13 recognised

-- Iteration 1: ISO-8859-14 --
-- ASCII String --
Encoding ISO-8859-14 recognised
-- Multibyte String --
Encoding ISO-8859-14 recognised

-- Iteration 1: ISO-8859-15 --
-- ASCII String --
Encoding ISO-8859-15 recognised
-- Multibyte String --
Encoding ISO-8859-15 recognised

-- Iteration 1: byte2be --
-- ASCII String --
Encoding byte2be recognised
-- Multibyte String --
Encoding byte2be recognised

-- Iteration 1: byte2le --
-- ASCII String --
Encoding byte2le recognised
-- Multibyte String --
Encoding byte2le recognised

-- Iteration 1: byte4be --
-- ASCII String --
Encoding byte4be recognised
-- Multibyte String --
Encoding byte4be recognised

-- Iteration 1: byte4le --
-- ASCII String --
Encoding byte4le recognised
-- Multibyte String --
Encoding byte4le recognised

-- Iteration 1: BASE64 --
-- ASCII String --
Encoding BASE64 recognised
-- Multibyte String --
Encoding BASE64 recognised

-- Iteration 1: HTML-ENTITIES --
-- ASCII String --
Encoding HTML-ENTITIES recognised
-- Multibyte String --
Encoding HTML-ENTITIES recognised

-- Iteration 1: 7bit --
-- ASCII String --
Encoding 7bit recognised
-- Multibyte String --
Encoding 7bit recognised

-- Iteration 1: 8bit --
-- ASCII String --
Encoding 8bit recognised
-- Multibyte String --
Encoding 8bit recognised

-- Iteration 1: EUC-CN --
-- ASCII String --
Encoding EUC-CN recognised
-- Multibyte String --
Encoding EUC-CN recognised

-- Iteration 1: CP936 --
-- ASCII String --
Encoding CP936 recognised
-- Multibyte String --
Encoding CP936 recognised

-- Iteration 1: HZ --
-- ASCII String --
Encoding HZ recognised
-- Multibyte String --
Encoding HZ recognised

-- Iteration 1: EUC-TW --
-- ASCII String --
Encoding EUC-TW recognised
-- Multibyte String --
Encoding EUC-TW recognised

-- Iteration 1: CP950 --
-- ASCII String --
Encoding CP950 recognised
-- Multibyte String --
Encoding CP950 recognised

-- Iteration 1: BIG-5 --
-- ASCII String --
Encoding BIG-5 recognised
-- Multibyte String --
Encoding BIG-5 recognised

-- Iteration 1: EUC-KR --
-- ASCII String --
Encoding EUC-KR recognised
-- Multibyte String --
Encoding EUC-KR recognised

-- Iteration 1: UHC --
-- ASCII String --
Encoding UHC recognised
-- Multibyte String --
Encoding UHC recognised

-- Iteration 1: ISO-2022-KR --
-- ASCII String --
Encoding ISO-2022-KR recognised
-- Multibyte String --
Encoding ISO-2022-KR recognised

-- Iteration 1: Windows-1251 --
-- ASCII String --
Encoding Windows-1251 recognised
-- Multibyte String --
Encoding Windows-1251 recognised

-- Iteration 1: Windows-1252 --
-- ASCII String --
Encoding Windows-1252 recognised
-- Multibyte String --
Encoding Windows-1252 recognised

-- Iteration 1: CP866 --
-- ASCII String --
Encoding CP866 recognised
-- Multibyte String --
Encoding CP866 recognised

-- Iteration 1: KOI8-R --
-- ASCII String --
Encoding KOI8-R recognised
-- Multibyte String --
Encoding KOI8-R recognised
Done