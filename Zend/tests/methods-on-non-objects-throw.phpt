--TEST--
Convert errors to exceptions from method calls on non-objects raise recoverable errors
--FILE--
<?php
set_error_handler(function($code, $message) {
  echo "Raising...\n";
  if (0 === strncmp('Call', $message, 4)) {
    throw new BadMethodCallException($message);
  } else if (0 === strncmp('Argument', $message, 8)) {
    throw new InvalidArgumentException($message);
  } else {
    trigger_error($message, E_USER_ERROR);
  }
}, E_RECOVERABLE_ERROR);

$x= null;
echo "Calling...\n";
try {
  $x->method();
} catch (BadMethodCallException $e) {
  echo "Caught expected ", $e->getMessage(), "!\n";
}
echo "Alive\n";
?>
--EXPECTF--
Calling...
Raising...
Caught expected Call to a member function method() on null!
Alive
