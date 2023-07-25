--TEST--
ReflectionFunction::getParameter()
--FILE--
<?php
class C {
    public function foo(string $bar) {}
}

$method = new ReflectionMethod('C', 'foo');
var_dump($method->getParameter(-1));
?>
--EXPECTF--
Fatal error: Uncaught ValueError: ReflectionFunctionAbstract::getParameter(): Argument #1 ($parameter) must be greater than or equal to 0 in %s:7
Stack trace:
#0 %s: ReflectionFunctionAbstract->getParameter(-1)
#1 {main}
  thrown in %s on line 7