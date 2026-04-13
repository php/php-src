--TEST--
Generic class: inheritance with wrong number of type arguments
--FILE--
<?php
class Box<T> {
    private $value;
    public function __construct(T $value) { $this->value = $value; }
}

class BadBox extends Box<int, string> {}
?>
--EXPECTF--
Fatal error: Class Box expects 1 generic type argument(s), 2 given in BadBox in %s on line %d
