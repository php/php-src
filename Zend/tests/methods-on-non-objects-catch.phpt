--TEST--
Catch method calls on non-objects raise recoverable errors
--FILE--
<?php
set_error_handler(function($code, $message) {
  var_dump($code, $message);
});

$x= null;
try {
    var_dump($x->method());
} catch (Error $e) {
  echo $e::class, ': ', $e->getCode(), ': ', $e->getMessage(), "\n";
}
echo "Alive\n";
?>
--EXPECTF--
Error: 0: Call to a member function method() on null
Alive
