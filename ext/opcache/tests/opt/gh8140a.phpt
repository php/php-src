--TEST--
GH-8140 (Wrong first class callable by name optimization)
--FILE--
<?php
namespace Test;

function greeter(string $name) {
  echo "Hello, {$name}!";
}

$mycallable = greeter(...);

$mycallable("world");
?>
--EXPECT--
Hello, world!
