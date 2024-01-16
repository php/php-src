--TEST--
mb_ucfirst(), mb_lcfirst functions tests
--EXTENSIONS--
mbstring
--FILE--
<?php
mb_internal_encoding("UTF-8");
echo "== Empty String ==\n";
var_dump(mb_ucfirst(""));
var_dump(mb_lcfirst(""));
echo "== mb_ucfirst ==\n";
var_dump(mb_ucfirst("ａｂ"));
var_dump(mb_ucfirst("ＡＢＳ"));
var_dump(mb_ucfirst("đắt quá!"));
echo "== mb_lcfirst ==\n";
var_dump(mb_lcfirst("ＡＢＳ"));
var_dump(mb_lcfirst("Xin chào"));
var_dump(mb_lcfirst("Đẹp quá!"));
?>
--EXPECT--
== Empty String ==
string(0) ""
string(0) ""
== mb_ucfirst ==
string(6) "Ａｂ"
string(9) "ＡＢＳ"
string(12) "Đắt quá!"
== mb_lcfirst ==
string(9) "ａＢＳ"
string(9) "xin chào"
string(12) "đẹp quá!"
