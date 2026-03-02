--TEST--
Bug GH-9248 (Segmentation fault in mb_strimwidth())
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(mb_strimwidth('The quick brown fox', 0, 8));
?>
--EXPECT--
string(8) "The quic"
