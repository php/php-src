--TEST--
Erasure: generic signature matches hand-erased equivalent
--FILE--
<?php
class Foo {}

class Generic<T : Foo> {
    public function get(): T { return new Foo; }
}

class Erased {
    public function get(): Foo { return new Foo; }
}

$rg = (new ReflectionClass('Generic'))->getMethod('get')->getReturnType()->__toString();
$re = (new ReflectionClass('Erased'))->getMethod('get')->getReturnType()->__toString();
var_dump($rg === $re);
?>
--EXPECT--
bool(true)
