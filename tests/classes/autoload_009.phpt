--TEST--
Ensure type hints for unknown types do not trigger autoload.
--FILE--
<?php
spl_autoload_register(function ($name) {
  echo "In autoload: ";
  var_dump($name);
});

function f(UndefClass $x)
{
}
f(new stdClass);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: f() expects argument #1 ($x) to be of type UndefClass, stdClass given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): f(Object(stdClass))
#1 {main}
  thrown in %s on line %d
