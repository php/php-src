--TEST--
Asymmetric visibility variations
--FILE--
<?php

class Test {
    public private(set) int $prop;
    public private(set) array $array;

    public function init() {
        $this->prop = 1;
        $this->array = [];
    }

    public function r() {
        echo $this->prop;
    }

    public function w() {
        $this->prop = 1;
        echo 'done';
    }

    public function rw() {
        $this->prop += 1;
        echo 'done';
    }

    public function im() {
        $this->array[] = 1;
        echo 'done';
    }

    public function is() {
        echo (int) isset($this->prop);
    }

    public function us() {
        unset($this->prop);
        echo 'done';
    }

    public function us_dim() {
        unset($this->array[0]);
        echo 'done';
    }
}

function r($test) {
    echo $test->prop;
}

function w($test) {
    $test->prop = 0;
    echo 'done';
}

function rw($test) {
    $test->prop += 1;
    echo 'done';
}

function im($test) {
    $test->array[] = 1;
    echo 'done';
}

function is($test) {
    echo (int) isset($test->prop);
}

function us($test) {
    unset($test->prop);
    echo 'done';
}

function us_dim($test) {
    unset($test->array[0]);
    echo 'done';
}

foreach ([true, false] as $init) {
    foreach ([true, false] as $scope) {
        foreach (['r', 'w', 'rw', 'im', 'is', 'us', 'us_dim'] as $op) {
            $test = new Test();
            if ($init) {
                $test->init();
            }

            echo 'Init: ' . ((int) $init) . ', scope: ' . ((int) $scope) . ', op: ' . $op . ": ";
            try {
                if ($scope) {
                    $test->{$op}();
                } else {
                    $op($test);
                }
            } catch (Error $e) {
                echo $e->getMessage();
            }
            echo "\n";
        }
    }
}

?>
--EXPECT--
Init: 1, scope: 1, op: r: 1
Init: 1, scope: 1, op: w: done
Init: 1, scope: 1, op: rw: done
Init: 1, scope: 1, op: im: done
Init: 1, scope: 1, op: is: 1
Init: 1, scope: 1, op: us: done
Init: 1, scope: 1, op: us_dim: done
Init: 1, scope: 0, op: r: 1
Init: 1, scope: 0, op: w: Cannot modify private(set) property Test::$prop from global scope
Init: 1, scope: 0, op: rw: Cannot modify private(set) property Test::$prop from global scope
Init: 1, scope: 0, op: im: Cannot indirectly modify private(set) property Test::$array from global scope
Init: 1, scope: 0, op: is: 1
Init: 1, scope: 0, op: us: Cannot unset private(set) property Test::$prop from global scope
Init: 1, scope: 0, op: us_dim: Cannot indirectly modify private(set) property Test::$array from global scope
Init: 0, scope: 1, op: r: Typed property Test::$prop must not be accessed before initialization
Init: 0, scope: 1, op: w: done
Init: 0, scope: 1, op: rw: Typed property Test::$prop must not be accessed before initialization
Init: 0, scope: 1, op: im: done
Init: 0, scope: 1, op: is: 0
Init: 0, scope: 1, op: us: done
Init: 0, scope: 1, op: us_dim: done
Init: 0, scope: 0, op: r: Typed property Test::$prop must not be accessed before initialization
Init: 0, scope: 0, op: w: Cannot modify private(set) property Test::$prop from global scope
Init: 0, scope: 0, op: rw: Typed property Test::$prop must not be accessed before initialization
Init: 0, scope: 0, op: im: Cannot indirectly modify private(set) property Test::$array from global scope
Init: 0, scope: 0, op: is: 0
Init: 0, scope: 0, op: us: Cannot unset private(set) property Test::$prop from global scope
Init: 0, scope: 0, op: us_dim: done
