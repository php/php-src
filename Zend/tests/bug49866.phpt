--TEST--
Bug #49866 (Making reference on string offsets crashes PHP)
--FILE--
<?php
$a = "string";
$b = &$a[1];
$b = "f";
echo $a;
--EXPECTF--
Fatal error: Uncaught Error: Cannot create references to/from string offsets in %sbug49866.php:3
Stack trace:
#0 {main}
  thrown in %sbug49866.php on line 3
