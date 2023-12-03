<?php

function get_zend_debug_type($v) {
    if ($v === true) {
        return 'true';
    }
    if ($v === false) {
        return 'false';
    }
    if (is_resource($v)) {
        return 'resource';
    }
    return get_debug_type($v);
}
function makeContainer($container) {
    if (is_array($container)) {
        return "[]";
    }
    if (is_resource($container)) {
        return "STDERR";
    }
    if ($container instanceof stdClass) {
        return "new stdClass()";
    }
    if ($container instanceof B) {
        return "new B()";
    }
    if ($container instanceof A) {
        return "new A()";
    }
    if ($container instanceof ArrayObject) {
        return "new ArrayObject()";
    }
    return var_export($container, true);
}
function makeOffset($offset) {
    if ($offset === PHP_INT_MIN) {
        return "PHP_INT_MIN";
    }
    if ($offset === PHP_INT_MAX) {
        return "PHP_INT_MAX";
    }
    return var_export($offset, true);
}

/* Taken from run-tests.php */
function expectf_to_regex(string $wanted): string
{
    $wanted_re = preg_quote($wanted);
    return strtr($wanted_re, [
        '%e' => preg_quote(DIRECTORY_SEPARATOR, '/'),
        '%s' => '[^\r\n]+',
        '%S' => '[^\r\n]*',
        '%a' => '.+',
        '%A' => '.*',
        '%w' => '\s*',
        '%i' => '[+-]?\d+',
        '%d' => '\d+',
        '%x' => '[0-9a-fA-F]+',
        '%f' => '[+-]?(?:\d+|(?=\.\d))(?:\.\d+)?(?:[Ee][+-]?\d+)?',
        '%F' => '([+-]?(?:\d+|(?=\.\d))(?:\.\d+)?(?:[Ee][+-]?\d+)?)|(NAN)|(INF)',
        '%c' => '.',
        '%0' => '\x00',
    ]);
}

class A implements ArrayAccess {
    public function offsetSet($offset, $value): void {
        var_dump(__METHOD__);
        var_dump($offset);
        var_dump($value);
    }
    public function offsetGet($offset): mixed {
        var_dump(__METHOD__);
        var_dump($offset);
        return "dummy";
    }
    public function offsetUnset($offset): void {
        var_dump(__METHOD__);
        var_dump($offset);
    }
    public function offsetExists($offset): bool {
        var_dump(__METHOD__);
        var_dump($offset);
        return false;
    }
}

class B extends ArrayObject {
    public function offsetSet($offset, $value): void {
        var_dump(__METHOD__);
        var_dump($offset);
        var_dump($value);
    }
    public function offsetGet($offset): mixed {
        var_dump(__METHOD__);
        var_dump($offset);
        return "dummy";
    }
    public function offsetUnset($offset): void {
        var_dump(__METHOD__);
        var_dump($offset);
    }
    public function offsetExists($offset): bool {
        var_dump(__METHOD__);
        var_dump($offset);
        return false;
    }
    public function append(mixed $value) : void{
        var_dump(__METHOD__);
        var_dump($value);
    }
}

$containers = [
    null,
    false,
    true,
    4,
    5.5,
    '10',
    '25.5',
    'string',
    [],
    STDERR,
    new stdClass(),
    new ArrayObject(),
    new A(),
    new B(),
];

$offsets = [
    null,
    false,
    true,
    4,
    5.5,
    6.0,
    //PHP_INT_MAX,
    //PHP_INT_MIN,
    PHP_INT_MAX * 2,
    PHP_INT_MIN * 2,
    INF,
    NAN,
    'string',
    '7',
    '8.5',
    '9.0',
    '2e3',
    '20a',
    '  20',
    '20  ',
    //"9179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368",
    //"-9179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368",
    "0x14",
    (string) PHP_INT_MAX * 2,
    (string) PHP_INT_MIN * 2,
];

$failures = [];
$failuresNb = 0;
$testCasesTotal = 0;

$var_dim_filename = __DIR__ . DIRECTORY_SEPARATOR . 'test_variable_offsets.inc';
