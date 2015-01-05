--TEST--
string offset 002
--FILE--
<?php
$a = "aaa";
$x = array(&$a[1]);
?>
--EXPECTF--
Fatal error: Cannot create references to/from string offsets in %sstr_offset_002.php on line 3
