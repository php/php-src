--TEST--
First Class Callable from Method
--FILE--
<?php
class Foo {
    public function bar()  {
        echo "OK";
    }
}

$foo = new Foo;
$bar = $foo->bar(...);

echo $bar();
?>
--EXPECT--
OK
