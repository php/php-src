--TEST--
ReflectionFunction::getParameter() errors
--FILE--
<?php
$function = new ReflectionFunction('sort');
function foo(string $bar) {}
function noParams() {}

try {
    var_dump($function->getParameter('Array'));
} catch(ReflectionException $e) {
    print($e->getMessage() . PHP_EOL);
}

try {
    var_dump($function->getParameter(-1));
} catch(ValueError $e) {
    print($e->getMessage() . PHP_EOL);
}

try {
    var_dump($function->getParameter(3));
} catch(ReflectionException $e) {
    print($e->getMessage() . PHP_EOL);
}

$function = new ReflectionFunction('foo');

try {
    var_dump($function->getParameter('Bar'));
} catch(ReflectionException $e) {
    print($e->getMessage() . PHP_EOL);
}

try {
    var_dump($function->getParameter(-1));
} catch(ValueError $e) {
    print($e->getMessage() . PHP_EOL);
}

try {
    var_dump($function->getParameter(1));
} catch(ReflectionException $e) {
    print($e->getMessage() . PHP_EOL);
}

$function = new ReflectionFunction('noParams');

try {
    var_dump($function->getParameter(1));
} catch(ReflectionException $e) {
    print($e->getMessage() . PHP_EOL);
}

?>
--EXPECT--
Function sort() has no parameter named "Array"
ReflectionFunctionAbstract::getParameter(): Argument #1 ($param) must be greater than or equal to 0
Function sort() has no parameter at offset 3
Function foo() has no parameter named "Bar"
ReflectionFunctionAbstract::getParameter(): Argument #1 ($param) must be greater than or equal to 0
Function foo() has no parameter at offset 1
Function noParams() has no parameters
