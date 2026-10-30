--TEST--
GH-17399 (iconv() leak on bailout)
--EXTENSIONS--
iconv
--INI--
memory_limit=2M
--FILE--
<?php
$str = str_repeat('a', 1024 * 1024);
iconv('UTF-8', 'ISO-8859-1', $str);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s in %s on line %d
