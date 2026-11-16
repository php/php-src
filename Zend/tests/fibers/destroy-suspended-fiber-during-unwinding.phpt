--TEST--
Destroying a suspended fiber during exception unwinding does not skip the enclosing catch block
--FILE--
<?php

class D {
    public function __destruct() {
        echo "D::__destruct\n";
    }
}

function suspended_fiber(): Fiber {
    $fiber = new Fiber(static function () {
        Fiber::suspend();
    });
    $fiber->start();

    return $fiber;
}

function destructor_first() {
    $d = new D();
    $fiber = suspended_fiber();
    throw new Exception('destructor_first');
}

function fiber_first() {
    $fiber = suspended_fiber();
    $d = new D();
    throw new Exception('fiber_first');
}

function throw_in_fiber_finally() {
    $d = new D();
    $fiber = new Fiber(static function () {
        try {
            Fiber::suspend();
        } finally {
            throw new Exception('fiber finally');
        }
    });
    $fiber->start();
    throw new Exception('throw_in_fiber_finally');
}

function catches(callable $f) {
    try {
        $f();
    } catch (Exception $e) {
        echo 'caught: ', $e->getMessage(), $e->getPrevious() ? ', previous: '.$e->getPrevious()->getMessage() : '', "\n";
    }
}

function catches_after_finally(callable $f) {
    try {
        try {
            $f();
        } finally {
            echo "finally\n";
        }
    } catch (Exception $e) {
        echo 'caught: ', $e->getMessage(), "\n";
    }
}

foreach (['destructor_first', 'fiber_first', 'throw_in_fiber_finally'] as $f) {
    echo "-- $f\n";
    try {
        catches($f);
        catches_after_finally($f);
    } catch (Throwable $e) {
        echo 'escaped: ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
-- destructor_first
D::__destruct
caught: destructor_first
D::__destruct
finally
caught: destructor_first
-- fiber_first
D::__destruct
caught: fiber_first
D::__destruct
finally
caught: fiber_first
-- throw_in_fiber_finally
D::__destruct
caught: fiber finally, previous: throw_in_fiber_finally
D::__destruct
finally
caught: fiber finally
