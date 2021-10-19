--TEST--
Bug #47704 (crashes on some "bad" operations with string offsets)
--FILE--
<?php
$s = "abd";
$s[0]->a += 1;
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot indirectly modify string offset in %s:%d
Stack trace:
#0 {main}
  thrown in %sbug47704.php on line %d
