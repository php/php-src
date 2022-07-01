--TEST--
Short closures syntax variations
--FILE--
<?php

$a = 1;

function printFunctionSignature($f) {
    $rf = new ReflectionFunction($f);
    $attrs = $rf->getAttributes();
    foreach ($attrs as $attr) {
        printf('#[%s] ', $attr->getName());
    }
    if ($rf->isStatic()) {
        print 'static ';
    }
    print 'fn ';
    if ($rf->returnsReference()) {
        print '& ';
    }
    print '(...)';
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

$a = 1;
$f = fn (): Foo {
    return $a;
};

printFunctionSignature($f);

$f = #[Attr] fn (): Foo {
};

printFunctionSignature($f);

--EXPECTF--
fn (...)
fn & (...)
static fn (...)
fn (...): Foo
fn (...): Foo
#[Attr] fn (...): Foo
