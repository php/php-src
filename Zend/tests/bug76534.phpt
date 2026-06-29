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
Fatal error: Uncaught Error: Cannot create references to/from string offsets in %s:%d
Stack trace:
#0 {main}

Next Exception: Illegal string offset "2bar" in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(2, 'Illegal string ...', '%s', 7)
#1 {main}
  thrown in %s on line %d