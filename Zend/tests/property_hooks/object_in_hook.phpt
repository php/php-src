--TEST--
Assign on object inside property hook is ok
--FILE--
<?php

class A implements ArrayAccess {
    public $b;

    public function offsetExists(mixed $offset): bool { return true; }
    public function offsetGet(mixed $offset): mixed { return $offset; }
    public function offsetSet(mixed $offset, $value): void {
        echo "Setting $offset => $value\n";
    }
    public function offsetUnset(mixed $offset): void {
        echo "Unsetting $offset\n";
    }
}

class C {
    public $a {
        get { return $this->a; }
        set { $this->a = $value; }
    }
}

$c = new C;
$c->a = new A();

$c->a->b = 'b';
var_dump($c->a->b);

var_dump($c->a['foo']);
$c->a['foo'] = 'foo';
unset($c->a['foo']);

?>
--EXPECT--
string(1) "b"
string(3) "foo"
Setting foo => foo
Unsetting foo
