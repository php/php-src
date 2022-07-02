--TEST--
Test mb_stristr() function : basic functionality
--EXTENSIONS--
mbstring
--FILE--
<?php
echo "*** Testing mb_stristr() : basic functionality ***\n";

mb_internal_encoding('UTF-8');

$string_ascii = 'abcdef';
$needle_ascii_upper = "BCD";
$needle_ascii_mixed = "bCd";
$needle_ascii_lower = "bcd";

//Greek string in lower case UTF-8
$string_mb = base64_decode('zrHOss6zzrTOtc62zrfOuM65zrrOu868zr3Ovs6/z4DPgc+Dz4TPhc+Gz4fPiM+J');
$needle_mb_upper = base64_decode('zpzOnc6ezp8=');
$needle_mb_lower = base64_decode('zrzOvc6+zr8=');
$needle_mb_mixed = base64_decode('zpzOnc6+zr8=');

echo "\n-- ASCII string: needle exists --\n";
var_dump(bin2hex(mb_stristr($string_ascii, $needle_ascii_upper, false, 'ISO-8859-1')));
var_dump(bin2hex(mb_stristr($string_ascii, $needle_ascii_lower)));
var_dump(bin2hex(mb_stristr($string_ascii, $needle_ascii_mixed, true)));


echo "\n-- ASCII string: needle doesn't exist --\n";
var_dump(mb_stristr($string_ascii, '123'));

echo "\n-- Multibyte string: needle exists --\n";
var_dump(bin2hex(mb_stristr($string_mb, $needle_mb_upper)));
var_dump(bin2hex(mb_stristr($string_mb, $needle_mb_lower, false, 'utf-8')));
var_dump(bin2hex(mb_stristr($string_mb, $needle_mb_mixed, true)));


echo "\n-- Multibyte string: needle doesn't exist --\n";
$needle2 = base64_decode("zrzOvs6/");
var_dump(mb_stristr($string_mb, $needle2));

?>
--EXPECT--
*** Testing mb_stristr() : basic functionality ***

-- ASCII string: needle exists --
string(10) "6263646566"
string(10) "6263646566"
string(2) "61"

-- ASCII string: needle doesn't exist --
bool(false)

-- Multibyte string: needle exists --
string(52) "cebccebdcebecebfcf80cf81cf83cf84cf85cf86cf87cf88cf89"
string(52) "cebccebdcebecebfcf80cf81cf83cf84cf85cf86cf87cf88cf89"
string(44) "ceb1ceb2ceb3ceb4ceb5ceb6ceb7ceb8ceb9cebacebb"

-- Multibyte string: needle doesn't exist --
bool(false)
