--TEST--
Check that __call() and __callStatic() work with named parameters
--FILE--
<?php

class Test {
    public function __call(string $method, array $args) {
        $this->{'_'.$method}(...$args);
    }

    public static function __callStatic(string $method, array $args) {
        (new static)->{'_'.$method}(...$args);
    }

    private function _method($a = 'a', $b = 'b') {
        echo "a: $a, b: $b\n";
    }
}

$obj = new class { public function __toString() { return "STR"; } };

$test = new Test;
$test->method(a: 'A', b: 'B');
$test->method(b: 'B');
$test->method(b: $obj);
Test::method(a: 'A', b: 'B');
Test::method(b: 'B');
Test::method(b: $obj);

?>
--EXPECT--
a: A, b: B
a: a, b: B
a: a, b: STR
a: A, b: B
a: a, b: B
a: a, b: STR
