--TEST--
Stack limit 002 - Stack limit checks with max_allowed_stack_size detection (fibers)
--EXTENSIONS--
zend_test
--INI--
fiber.stack_size=512k
--FILE--
<?php

var_dump(zend_test_zend_call_stack_get());

class Test1 {
    public function __destruct() {
        new Test1;
    }
}

class Test2 {
    public function __clone() {
        clone $this;
    }
}

class Test3 {
    public function __sleep()
    {
        serialize($this);
    }
}

function replace() {
    return preg_replace_callback('#.#', function () {
        return replace();
    }, 'x');
}

$fiber = new Fiber(function (): void {
    try {
        new Test1;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        clone new Test2;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        serialize(new Test3);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        replace();
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
});

$fiber->start();

?>
--EXPECTF--
array(4) {
  ["base"]=>
  string(%d) "0x%x"
  ["max_size"]=>
  string(%d) "0x%x"
  ["position"]=>
  string(%d) "0x%x"
  ["EG(stack_limit)"]=>
  string(%d) "0x%x"
}
Maximum call stack size of %d bytes reached. Infinite recursion?
Maximum call stack size of %d bytes reached. Infinite recursion?
Maximum call stack size of %d bytes reached. Infinite recursion?
Maximum call stack size of %d bytes reached. Infinite recursion?
