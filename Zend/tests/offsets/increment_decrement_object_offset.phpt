--TEST--
increment/decrement on proper container objects
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

class Legacy implements ArrayAccess {
    private int $i = 15;
    public function offsetSet($offset, $value): void {
        var_dump(__METHOD__);
        $this->i = $value;
    }
    public function offsetGet($offset): mixed {
        var_dump(__METHOD__);
        return $this->i;
    }
    public function offsetUnset($offset): void {
        var_dump(__METHOD__);
    }
    public function offsetExists($offset): bool {
        var_dump(__METHOD__);
        return true;
    }
}

class Container implements DimensionFetchable, DimensionWritable {
    private int $i = 15;

    public function offsetExists($offset): bool {
        var_dump(__METHOD__);
        return true;
    }
    public function offsetGet($offset): mixed {
        var_dump(__METHOD__);
        return $this->i;
    }
    public function offsetSet($offset, $value): void {
        var_dump(__METHOD__);
        $this->i = $value;
    }
    public function &offsetFetch($offset): mixed {
        var_dump(__METHOD__);
        return $this->i;
    }
}

foreach ([new Legacy(), new Container()] as $container) {
    echo zend_test_var_export($container), '[0]', "\n";

    try {
        var_dump($container[0]++);
        var_dump($container[0]);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    try {
        var_dump($container[0]--);
        var_dump($container[0]);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECTF--
new Legacy()[0]
string(20) "Legacy::offsetExists"
string(17) "Legacy::offsetGet"

Notice: Indirect modification of overloaded element of Legacy has no effect in %s on line %d
int(15)
string(17) "Legacy::offsetGet"
int(15)
string(20) "Legacy::offsetExists"
string(17) "Legacy::offsetGet"

Notice: Indirect modification of overloaded element of Legacy has no effect in %s on line %d
int(15)
string(17) "Legacy::offsetGet"
int(15)
new Container()[0]
string(23) "Container::offsetExists"
string(22) "Container::offsetFetch"
int(15)
string(20) "Container::offsetGet"
int(16)
string(23) "Container::offsetExists"
string(22) "Container::offsetFetch"
int(16)
string(20) "Container::offsetGet"
int(15)
