--TEST--
First Class Callable from Internal
--FILE--
<?php
$sprintf = sprintf(...);

echo $sprintf("Hello %s", "World");
?>
--EXPECT--
Hello World
