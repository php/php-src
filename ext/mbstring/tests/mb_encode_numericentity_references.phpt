--TEST--
mb_encode_numericentity() reference handling
--EXTENSIONS--
mbstring
--FILE--
<?php
$n = 0;
$convmap = [&$n, 0x1FFFFF, 0, 0x10FFFF];
var_dump(mb_encode_numericentity("", $convmap, "utf8"));
?>
--EXPECT--
string(0) ""
