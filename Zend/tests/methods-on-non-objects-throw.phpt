--TEST--
Convert errors to exceptions from method calls on non-objects raise recoverable errors
--FILE--
<?php
set_error_handler(function($code, $message) {
  if (0 === strncmp('Call', $message, 4)) {
    throw new BadMethodCallException($message);
  } else if (0 === strncmp('Argument', $message, 8)) {
    throw new InvalidArgumentException($message);
  } else {
    trigger_error($message, E_USER_ERROR);
  }
}, E_RECOVERABLE_ERROR);

$x= null;
try {
  $x->method();
} catch (BadMethodCallException $e) {
  echo "Caught expected ", $e->getMessage(), "!\n";
}
echo "Alive\n";
?>
--EXPECTF--
Caught expected Call to a member function method() on a non-object!
Alive
