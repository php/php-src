--TEST--
Ensure type hints for unknown types do not trigger autoload. 
--FILE--
<?php
  function __autoload($name)
  {
      echo "In autoload: ";
      var_dump($name);
  }
  
  function f(UndefClass $x)
  {
  }
  f(new stdClass);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to f() must be an instance of UndefClass, instance of stdClass given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): f(Object(stdClass))
#1 {main}
  thrown in %s on line %d
