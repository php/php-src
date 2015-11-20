--TEST--
Bug #70944 (try{ } finally{} can create infinite chains of exceptions)
--FILE--
<?php
$e = new Exception("Bar");
try {
	  throw new Exception("Foo", 0, $e);
} finally {
	  throw $e;
}
?>
--EXPECTF--
Fatal error: Uncaught Exception: Bar in %sbug70944.php:%d
Stack trace:
#0 {main}
  thrown in %sbug70944.php on line %d
