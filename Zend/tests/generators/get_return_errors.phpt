--TEST--
Generator::getReturn() error cases
--FILE--
<?php

function gen1() {
    yield 1;
    yield 2;
    return 3;
}

$gen = gen1();
try {
    // Generator hasn't reached the "return" yet
    $gen->getReturn();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

function gen2() {
    throw new Exception("gen2() throw");
    yield 1;
    return 2;
}

$gen = gen2();
try {
    $gen->next();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
try {
    // Generator has been aborted as a result of an exception
    $gen->getReturn();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

function gen3() {
    throw new Exception("gen3() throw");
    return 1;
    yield 2;
}

$gen = gen3();
try {
    // Generator throws during auto-priming of getReturn() call
    $gen->getReturn();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

function gen4() {
    yield;
    return 1;
}

$gen = gen4();
try {
    $gen->throw(new Exception("gen4() throw"));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
try {
    // Generator has been aborted as a result of an exception
    // that was injected using throw()
    $gen->getReturn();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot get return value of a generator that hasn't returned
gen2() throw
Cannot get return value of a generator that hasn't returned
gen3() throw
gen4() throw
Cannot get return value of a generator that hasn't returned
