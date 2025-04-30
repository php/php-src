--TEST--
Attempted read/write of backing value in a delegated method throws
--SKIPIF--
<?php
if (!function_exists('zend_test_zend_call_stack_get')) die("skip zend_test_zend_call_stack_get() is not available");
?>
--EXTENSIONS--
zend_test
--INI--
; The test may use a large amount of memory on systems with a large stack limit
memory_limit=2G
--FILE--
<?php

class Test {
    public $prop = 42 {
        get => $this->getProp($this->prop);
        set {
            $this->setProp($this->prop, $value);
        }
    }

    private function getProp($prop) {
        return $this->prop;
    }

    private function setProp($prop, $value) {
        $this->prop = $value;
    }

    public $prop2 = 42 {
        get => $this->prop2;
        set => $value;
    }
}

class Child extends Test {
    public $prop2 = 42 {
        get => parent::$prop2::get();
        set { parent::$prop2::set($value); }
    }
}

$test = new Test;

try {
    $test->prop = 0;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump($test->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$child = new Child();
$child->prop2 = 43;
var_dump($child->prop2);

?>
--EXPECTF--
Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
int(43)
