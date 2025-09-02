--TEST--
The SensitiveParameterValue replacement value correctly captures the original value.
--FILE--
<?php

function test(
    $foo,
    #[SensitiveParameter] $bar,
    $baz
) {
    throw new Exception('Error');
}

try {
    test('foo', 'bar', 'baz');
    echo 'Not reached';
} catch (Exception $e) {
    echo $e->getMessage(), PHP_EOL;
    $testFrame = $e->getTrace()[0];
    var_dump($testFrame['function']);
    var_dump(count($testFrame['args']));
    var_dump($testFrame['args'][0]);
    assert($testFrame['args'][1] instanceof SensitiveParameterValue);
    var_dump($testFrame['args'][1]->getValue());
    var_dump($testFrame['args'][2]);
    echo "Success", PHP_EOL;
}

function test2(
    $foo,
    #[SensitiveParameter] ...$variadic,
) {
    throw new Exception('Error 2');
}

try {
    test2('foo', 'variadic1', 'variadic2', 'variadic3');
    echo 'Not reached';
} catch (Exception $e) {
    echo $e->getMessage(), PHP_EOL;
    $testFrame = $e->getTrace()[0];
    var_dump($testFrame['function']);
    var_dump(count($testFrame['args']));
    var_dump($testFrame['args'][0]);
    assert($testFrame['args'][1] instanceof SensitiveParameterValue);
    var_dump($testFrame['args'][1]->getValue());
    assert($testFrame['args'][2] instanceof SensitiveParameterValue);
    var_dump($testFrame['args'][2]->getValue());
    assert($testFrame['args'][3] instanceof SensitiveParameterValue);
    var_dump($testFrame['args'][3]->getValue());
    echo "Success", PHP_EOL;
}

?>
--EXPECTF--
Error
string(4) "test"
int(3)
string(3) "foo"
string(3) "bar"
string(3) "baz"
Success
Error 2
string(5) "test2"
int(4)
string(3) "foo"
string(9) "variadic1"
string(9) "variadic2"
string(9) "variadic3"
Success
