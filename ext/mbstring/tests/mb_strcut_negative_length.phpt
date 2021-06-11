--TEST--
mb_strcut() negative length test
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(mb_strcut('Déjà vu', 1, -3));
var_dump(mb_strcut('Déjà vu', 1, -4));
var_dump(mb_strcut('Déjà vu', 1, -5));
var_dump(mb_strcut('Déjà vu', 1, -6));
var_dump(mb_strcut('Déjà vu', 1, -999));
?>
--EXPECT--
string(5) "éjà"
string(3) "éj"
string(3) "éj"
string(2) "é"
string(0) ""
