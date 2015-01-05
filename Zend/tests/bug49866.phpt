--TEST--
Bug #49866 (Making reference on string offsets crashes PHP)
--FILE--
<?php
$a = "string";
$b = &$a[1];
$b = "f";
echo $a;
--EXPECTF--
Fatal error: Cannot create references to/from string offsets nor overloaded objects in %sbug49866.php on line 3
