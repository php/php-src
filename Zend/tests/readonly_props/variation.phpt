--TEST--
Readonly variations
--FILE--
<?php

class Test {
    public readonly int $prop;

    public function init() {
        $this->prop = 1;
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
        $this->prop[] = 1;
        echo 'done';
    }

    public function is() {
        echo (int) isset($this->prop);
    }

    public function us() {
        unset($this->prop);
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
    $test->prop[] = 1;
    echo 'done';
}

function is($test) {
    echo (int) isset($test->prop);
}

function us($test) {
    unset($test->prop);
    echo 'done';
}

foreach ([true, false] as $init) {
    foreach ([true, false] as $scope) {
        foreach (['r', 'w', 'rw', 'im', 'is', 'us'] as $op) {
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
Init: 1, scope: 1, op: w: Cannot modify readonly property Test::$prop
Init: 1, scope: 1, op: rw: Cannot modify readonly property Test::$prop
Init: 1, scope: 1, op: im: Cannot modify readonly property Test::$prop
Init: 1, scope: 1, op: is: 1
Init: 1, scope: 1, op: us: Cannot unset readonly property Test::$prop
Init: 1, scope: 0, op: r: 1
Init: 1, scope: 0, op: w: Cannot modify readonly property Test::$prop
Init: 1, scope: 0, op: rw: Cannot modify readonly property Test::$prop
Init: 1, scope: 0, op: im: Cannot modify readonly property Test::$prop
Init: 1, scope: 0, op: is: 1
Init: 1, scope: 0, op: us: Cannot unset readonly property Test::$prop
Init: 0, scope: 1, op: r: Typed property Test::$prop must not be accessed before initialization
Init: 0, scope: 1, op: w: done
Init: 0, scope: 1, op: rw: Typed property Test::$prop must not be accessed before initialization
Init: 0, scope: 1, op: im: Cannot indirectly modify readonly property Test::$prop
Init: 0, scope: 1, op: is: 0
Init: 0, scope: 1, op: us: done
Init: 0, scope: 0, op: r: Typed property Test::$prop must not be accessed before initialization
Init: 0, scope: 0, op: w: Cannot initialize readonly property Test::$prop from global scope
Init: 0, scope: 0, op: rw: Typed property Test::$prop must not be accessed before initialization
Init: 0, scope: 0, op: im: Cannot indirectly modify readonly property Test::$prop
Init: 0, scope: 0, op: is: 0
Init: 0, scope: 0, op: us: Cannot unset readonly property Test::$prop from global scope
