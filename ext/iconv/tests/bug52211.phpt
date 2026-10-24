--TEST--
Bug #52211 (iconv() returns part of string on error)
--EXTENSIONS--
iconv
--FILE--
<?php

// The input contains an incomplete UTF-8 sequence: 0xC3 must be followed
// by a continuation byte, but is followed by "D". Unlike characters that
// are merely unrepresentable in the target charset (which some iconv
// implementations replace instead of rejecting), an invalid source
// sequence makes every implementation fail.
$str = "PATHOLOGIES M\xC3DICO-CHIRUR. ADUL. PL";
$str_iconv = iconv("UTF-8", "ASCII", $str);
var_dump($str_iconv);

?>
--EXPECTF--
Notice: iconv(): Detected an illegal character in input string in %s on line %d
bool(false)
