--TEST--
Crash when calling a method of a class that inherits PDOStatement
--EXTENSIONS--
pdo
--FILE--
<?php

class MyStatement extends PDOStatement
{
}

$obj = new MyStatement;
var_dump($obj->foo());

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined method MyStatement::foo() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
