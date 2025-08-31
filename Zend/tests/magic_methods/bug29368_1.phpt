--TEST--
Bug #29368.1 (The destructor is called when an exception is thrown from the constructor).
--FILE--
<?php
function throwme($arg)
{
    throw new Exception;
}

class foo {
  function __construct() {
    echo "Inside constructor\n";
    throwme($this);
  }

  function __destruct() {
    echo "Inside destructor\n";
  }
}

try {
  $bar = new foo;
} catch(Exception $exc) {
  echo "Caught exception!\n";
}
?>
--EXPECT--
Inside constructor
Caught exception!
