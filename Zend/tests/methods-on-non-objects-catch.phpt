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
  var_dump($e->getCode(), $e->getMessage());
}
echo "Alive\n";
?>
--EXPECT--
int(0)
string(31) "Call to method method() on null"
Alive
