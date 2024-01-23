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
The parameter specified by its name could not be found
ReflectionFunctionAbstract::getParameter(): Argument #1 ($parameter) must be greater than or equal to 0
The parameter specified by its offset could not be found
The parameter specified by its name could not be found
ReflectionFunctionAbstract::getParameter(): Argument #1 ($parameter) must be greater than or equal to 0
The parameter specified by its offset could not be found
Function has no parameters
