--TEST--
Bug #81693 (mb_check_encoding(7bit) segfaults)
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(mb_check_encoding('Hello world', '7bit'));
?>
--EXPECT--
bool(true)
