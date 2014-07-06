--TEST--
Catch method calls on non-objects without using return value
--FILE--
<?php
set_error_handler(function($code, $message) {
  echo "Caught\n";
});

$x= null;
$x->method();
echo "Alive\n";
?>
--EXPECTF--
Caught
Alive
