--TEST--
GH-8140 (Wrong first class callable by name optimization)
--FILE--
<?php
$mycallable = greeter(...);

function greeter(string $name) {
    echo "Hello, {$name}!";
}
  
$mycallable("world");
?>
--EXPECT--
Hello, world!
