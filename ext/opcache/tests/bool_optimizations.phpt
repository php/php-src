--TEST--
Test result of various optimizations
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function testAlwaysReturnsTrue(array $values) {
    if (count($values) > 0) {
        return true;
    }
    return true;
}

function testJmpzOptimization(array $values) : bool {
    if (count($values) == 0) {
        return true;
    }
    return false;
}

function testJmpnzOptimization(bool $i) : bool {
    if (!$i) {
        return false;
    }
    return true;
}

function testJmpzNotOptimizedCast(array $values) : int {
    if (count($values) > 0) {
        return true;
    }
    return false;
}

function testJmpzNotOptimizedThrow(array $values) : int {
    if (count($values) > 0) {
        return [];
    }
    return new stdClass();
}

var_dump(testAlwaysReturnsTrue([]));
var_dump(testAlwaysReturnsTrue([2]));
var_dump(testJmpnzOptimization(false));
var_dump(testJmpnzOptimization(true));
var_dump(testJmpzOptimization([]));
var_dump(testJmpzOptimization([2]));
echo "Testing cases without optimization\n";
foreach ([[], [[]]] as $x) {
    try {
        var_dump(testJmpzNotOptimizedCast($x));
    } catch (TypeError $e) {
        echo "TypeError at line {$e->getLine()}: {$e->getMessage()}\n";
    }
}
foreach ([[], [[]]] as $x) {
    try {
        var_dump(testJmpzNotOptimizedThrow($x));
    } catch (TypeError $e) {
        echo "TypeError at line {$e->getLine()}: {$e->getMessage()}\n";
    }
}

--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
Testing cases without optimization
int(0)
int(1)
TypeError at line 34: Return value of testJmpzNotOptimizedThrow() must be of the type int, object returned
TypeError at line 32: Return value of testJmpzNotOptimizedThrow() must be of the type int, array returned
