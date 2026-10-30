--TEST--
ReflectionMethod::createFromMethodName() with an anonymous class name
--FILE--
<?php

$obj = new class {
    public function m() { return 42; }
};

$name = (new ReflectionClass($obj))->getName();
var_dump(str_contains($name, "\0"));

$m = ReflectionMethod::createFromMethodName($name . '::m');
var_dump($m->getName(), $m->invoke($obj));

?>
--EXPECT--
bool(true)
string(1) "m"
int(42)
