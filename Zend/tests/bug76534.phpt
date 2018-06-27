--TEST--
Bug #76534 (PHP hangs on 'illegal string offset on string references with an error handler)
--FILE--
<?php
set_error_handler(function ($severity, $message, $file, $line) {
	throw new \Exception($message);
});

$x = "foo";
$y = &$x["bar"];
?>
--EXPECTF--
Fatal error: Uncaught Exception: Illegal string offset 'bar' in %sbug76534.php:%d
Stack trace:
#0 %sbug76534.php(%d): {closure}(2, 'Illegal string ...', '%s', %d, Array)
#1 {main}
  thrown in %sbug76534.php on line %d
