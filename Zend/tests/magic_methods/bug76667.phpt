--TEST--
Bug #76667 (Segfault with divide-assign op and __get + __set)
--FILE--
<?php

class T {
    public function __get($k)
    {
        return $undefined->$k;
    }

    public function __set($k, $v)
    {
        return $this->$v /= 0;
    }
}

$x = new T;
try {
    $x->x = 1;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: Undefined variable $undefined in %s on line %d

Warning: Attempt to read property "1" on null in %s on line %d
DivisionByZeroError: Division by zero
