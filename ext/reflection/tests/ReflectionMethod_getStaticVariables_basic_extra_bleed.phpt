--TEST--
ReflectionMethod::getStaticVariables() should not bleed IS_TYPE_UNINITIALIZED
--FILE--
<?php

function test() {
    echo "test() called\n";
    return 42;
}

function foo() {
    $methodInfo = new ReflectionFunction(__FUNCTION__);
    $nullWithIsTypeUninitialized = $methodInfo->getStaticVariables()['a'];

    static $a = test();
    var_dump($a);

    // Technically, IS_TYPE_UNINITIALIZED does bleed, but it doesn't matter since there's no way we
    // can assign it to the static variable directly instead of the reference.
    $staticVar = &$methodInfo->getStaticVariables()['a'];
    $staticVar = $nullWithIsTypeUninitialized;
}

foo();
foo();

?>
--EXPECT--
test() called
int(42)
NULL
