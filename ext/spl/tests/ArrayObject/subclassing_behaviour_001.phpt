--TEST--
Subclassing ArrayObject should still call overridden offsetSet method when appending
--EXTENSIONS--
spl
--FILE--
<?php
class A implements ArrayAccess {
    public function offsetSet($offset, $value): void {
        var_dump(__METHOD__);
        var_dump($offset);
        var_dump($value);
    }
    public function offsetGet($offset): mixed {}
    public function offsetUnset($offset): void {}
    public function offsetExists($offset): bool {}
}

class B extends ArrayObject {
    public function offsetSet($offset, $value): void {
        var_dump(__METHOD__);
        var_dump($offset);
        var_dump($value);
    }
    public function append(mixed $value) : void{
        var_dump(__METHOD__);
        var_dump($value);
    }
}

$a = new A();
$a[] = 1;

$b = new B();
$b[] = 1;
?>
--EXPECT--
string(12) "A::offsetSet"
NULL
int(1)
string(9) "B::append"
int(1)
