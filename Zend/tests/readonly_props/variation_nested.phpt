--TEST--
Readonly nested variations
--FILE--
<?php

class Inner {
    public int $prop = 1;
    public array $array = [];
}

class Test {
    public readonly Inner $prop;

    public function init() {
        $this->prop = new Inner();
    }
}

function r($test) {
    echo $test->prop->prop;
}

function w($test) {
    $test->prop->prop = 0;
    echo 'done';
}

function rw($test) {
    $test->prop->prop += 1;
    echo 'done';
}

function im($test) {
    $test->prop->array[] = 1;
    echo 'done';
}

function is($test) {
    echo (int) isset($test->prop->prop);
}

function us($test) {
    unset($test->prop->prop);
    echo 'done';
}

foreach ([true, false] as $init) {
    foreach (['r', 'w', 'rw', 'im', 'is', 'us'] as $op) {
        $test = new Test();
        if ($init) {
            $test->init();
        }

        echo 'Init: ' . ((int) $init) . ', op: ' . $op . ": ";
        try {
            $op($test);
        } catch (Error $e) {
            echo $e->getMessage();
        }
        echo "\n";
    }
}

?>
--EXPECT--
Init: 1, op: r: 1
Init: 1, op: w: done
Init: 1, op: rw: done
Init: 1, op: im: done
Init: 1, op: is: 1
Init: 1, op: us: done
Init: 0, op: r: Typed property Test::$prop must not be accessed before initialization
Init: 0, op: w: Cannot indirectly modify readonly property Test::$prop
Init: 0, op: rw: Typed property Test::$prop must not be accessed before initialization
Init: 0, op: im: Cannot indirectly modify readonly property Test::$prop
Init: 0, op: is: 0
Init: 0, op: us: done
