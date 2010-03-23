--TEST--
Test mb_encode_mimeheader() function : basic functionality
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_encode_mimeheader') or die("skip mb_encode_mimeheader() is not available in this build");
?>

--FILE--
<?php
/* Prototype  : string mb_encode_mimeheader
 * (string $str [, string $charset [, string $transfer-encoding [, string $linefeed [, int $indent]]]])
 * Description: Converts the string to MIME "encoded-word" in the format of =?charset?(B|Q)?encoded_string?= 
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Test basic functionality of mb_encode_mimeheader with different strings.
 * For the below strings:
 * 'English' is ASCII only, 'Japanese' has no ASCII characters and 'Greek' is mixed.
 */

echo "*** Testing mb_encode_mimeheader() : basic ***\n";

$english = array('English' => 'This is an English string. 0123456789');
$nonEnglish = array('Japanese' => base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC'),
                'Greek' => base64_decode('zpHPhc+Ez4wgzrXOr869zrHOuSDOtc67zrvOt869zrnOus+MIM66zrXOr868zrXOvc6/LiAwMTIzNDU2Nzg5Lg=='));

foreach ($english as $lang => $input) {
	echo "\nLanguage: $lang\n";
	echo "-- Base 64: --\n";
	var_dump(mb_encode_mimeheader($input, 'UTF-8', 'B'));
	echo "-- Quoted-Printable --\n";
	var_dump(mb_encode_mimeheader($input, 'UTF-8', 'Q'));
}

mb_internal_encoding('utf-8');

foreach ($nonEnglish as $lang => $input) {
	echo "\nLanguage: $lang\n";
	echo "-- Base 64: --\n";
	var_dump(mb_encode_mimeheader($input, 'UTF-8', 'B'));
	echo "-- Quoted-Printable --\n";
	var_dump(mb_encode_mimeheader($input, 'UTF-8', 'Q'));
}

echo "Done";
?>
--EXPECTF--
*** Testing mb_encode_mimeheader() : basic ***

Language: English
-- Base 64: --
string(37) "This is an English string. 0123456789"
-- Quoted-Printable --
string(37) "This is an English string. 0123456789"

Language: Japanese
-- Base 64: --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="
-- Quoted-Printable --
string(117) "=?UTF-8?Q?=E6=97=A5=E6=9C=AC=E8=AA=9E=E3=83=86=E3=82=AD=E3=82=B9=E3=83=88?=
 =?UTF-8?Q?=E3=81=A7=E3=81=99=E3=80=82?="

Language: Greek
-- Base 64: --
string(115) "=?UTF-8?B?zpHPhc+Ez4wgzrXOr869zrHOuSDOtc67zrvOt869zrnOus+MIM66zrXOr868?=
 =?UTF-8?B?zrXOvc6/LiAwMTIzNDU2Nzg5Lg==?="
-- Quoted-Printable --
string(249) "=?UTF-8?Q?=CE=91=CF=85=CF=84=CF=8C=20=CE=B5=CE=AF=CE=BD=CE=B1=CE=B9=20?=
 =?UTF-8?Q?=CE=B5=CE=BB=CE=BB=CE=B7=CE=BD=CE=B9=CE=BA=CF=8C=20=CE=BA=CE=B5?=
 =?UTF-8?Q?=CE=AF=CE=BC=CE=B5=CE=BD=CE=BF=2E=20=30=31=32=33=34=35=36=37=38?=
 =?UTF-8?Q?=39=2E?="
Done