--TEST--
mb_strcut() doesn't over allocate - length argument
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
--FILE--
<?php
var_dump(mb_strcut("abc", 0, 100 * 1024 * 1024, "UTF-7"));
?>
--EXPECT--
string(3) "abc"
