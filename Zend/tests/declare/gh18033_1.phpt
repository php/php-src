--TEST--
GH-18033 (NULL-ptr dereference when using register_tick_function in destructor)
--DESCRIPTION--
Needs --repeat 2 or something similar to reproduce
--CREDITS--
clesmian
--FILE--
<?php
class Foo {
  function __destruct() {
    declare(ticks=1);
    register_tick_function(
       function() { }
    );
    echo "In destructor\n";
  }
}

$bar = new Foo;
echo "Done\n";
?>
--EXPECT--
Done
In destructor
