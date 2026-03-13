--TEST--
GH-13815 (mb_trim() inaccurate $characters default value)
--EXTENSIONS--
mbstring
--FILE--
<?php
$strUtf8 = "\u{3042}\u{3000}"; // U+3000: fullwidth space
var_dump(mb_strlen(mb_trim($strUtf8)));
var_dump(mb_strlen(mb_trim($strUtf8, encoding: 'UTF-8')));

mb_internal_encoding('Shift_JIS');
$strSjis = mb_convert_encoding($strUtf8, 'Shift_JIS', 'UTF-8');
var_dump(mb_strlen(mb_trim($strSjis)));
var_dump(mb_strlen(mb_trim($strSjis, encoding: 'Shift_JIS')));
?>
--EXPECTF--
int(1)
int(1)

Deprecated: Function mb_internal_encoding() is deprecated since 8.5, use internal_encoding INI settings instead in %s on line %d
int(1)
int(1)
