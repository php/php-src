--TEST--
Bug #76534 (PHP hangs on 'illegal string offset on string references with an error handler)
--FILE--
<?php
set_error_handler(function ($severity, $message, $file, $line) {
    throw new \Exception($message);
});

$x = "foo";
$y = &$x["2bar"];
?>
--EXPECTF--
Fatal error: Uncaught Exception: Illegal string offset "2bar" in %s:%d
Stack trace:
#0 %sbug76534.php(%d): {closure}(2, 'Illegal string ...', '%s', %d)
#1 {main}
  thrown in %sbug76534.php on line %d
