--TEST--
First Class Callable from Private Scope
--FILE--
<?php
class Foo {
    private function method() {
        return "OK";
    }
    
    public function bar()  {
        return $this->method(...);
    }
}

$foo = new Foo;
$bar = $foo->bar(...);

echo ($bar())();
?>
--EXPECT--
OK
