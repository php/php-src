--TEST--
list<T> parameter reflection string
--FILE--
<?php
class User {}

function f(list<User> $users): void {}

$param = (new ReflectionFunction('f'))->getParameters()[0];
echo (string) $param->getType(), "\n";
?>
--EXPECT--
list<User>
