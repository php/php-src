--TEST--
Short closures syntax variations
--FILE--
<?php

$a = 1;

function printFunctionSignature($f) {
    $rf = new ReflectionFunction($f);
    if ($rf->isStatic()) {
        print 'static ';
    }
    print 'fn ';
    if ($rf->returnsReference()) {
        print '& ';
    }
    print '(...)';
    $usedVars = $rf->getClosureUsedVariables();
    if (count($usedVars) > 0) {
        print ' use (';
        $n = 0;
        foreach ($usedVars as $var => $_) {
            if ($n++ > 0) {
                print ', ';
            }
            print $var;
        }
        print ')';
    }
    $type = $rf->getReturnType();
    if ($type !== null) {
        print ': ' . $type->getName();
    }
    print "\n";
};

$f = fn () {
    return 1;
};

printFunctionSignature($f);

$f = fn & () {
    return 1;
};

printFunctionSignature($f);

$f = static fn () {
    return 1;
};

printFunctionSignature($f);

$f = fn (): Foo {
    return 1;
};

printFunctionSignature($f);

$f = fn () use ($a) {
};

printFunctionSignature($f);

$f = fn () use ($a): Foo {
};

printFunctionSignature($f);

--EXPECTF--
fn (...)
fn & (...)
static fn (...)
fn (...): Foo
fn (...) use (a)
fn (...) use (a): Foo
