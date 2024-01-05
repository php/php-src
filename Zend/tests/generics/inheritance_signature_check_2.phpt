--TEST--
Validating signatures involving generic parameters
--FILE--
<?php

abstract class Test<T> {
    public function method(T $param): T {}
}

class Test2 extends Test<int> {
    public function method(int $param): int {}
}

class Test3<T> extends Test<T> {
    public function method(T $param): T {}
}

class Test4<T> extends Test<T> {
    public function method(?T $param): T {}
}

?>
===DONE===
--EXPECT--
===DONE===
