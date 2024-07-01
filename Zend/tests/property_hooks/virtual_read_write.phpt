--TEST--
Attempted read/write of virtual property backing value throws
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('skip ASAN reports stack-overflow');
?>
--INI--
; The test may use a large amount of memory on systems with a large stack limit
memory_limit=2G
--FILE--
<?php

class Test {
    public $prop {
        get => $this->getProp();
        set {
            $this->setProp($value);
        }
    }

    private function getProp() {
        return $this->prop;
    }

    private function setProp($value) {
        $this->prop = $value;
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

?>
--EXPECTF--
Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
