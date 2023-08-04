--TEST--
ReflectionFunction::hasParameter() errors
--FILE--
<?php
function foo(string $bar) {}

$function = new ReflectionFunction('sort');

try {
    var_dump($function->hasParameter(-1));
} catch (ValueError $e) {
    print($e->getMessage() . PHP_EOL);
}

$function = new ReflectionFunction('foo');

try {
    var_dump($function->hasParameter(-1));
} catch (ValueError $e) {
    print($e->getMessage() . PHP_EOL);
}
?>
--EXPECT--
ReflectionFunctionAbstract::hasParameter(): Argument #1 ($parameter) must be greater than or equal to 0
ReflectionFunctionAbstract::hasParameter(): Argument #1 ($parameter) must be greater than or equal to 0
