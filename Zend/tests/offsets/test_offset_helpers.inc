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
function zend_test_var_export($value) {
    if ($value === PHP_INT_MIN) {
        return "PHP_INT_MIN";
    }
    if ($value === PHP_INT_MAX) {
        return "PHP_INT_MAX";
    }
    if (is_array($value)) {
        return "[]";
    }
    if (is_resource($value)) {
        return "STDERR";
    }
    if ($value instanceof stdClass) {
        return "new stdClass()";
    }
    if ($value instanceof B) {
        return "new B()";
    }
    if ($value instanceof A) {
        return "new A()";
    }
    if ($value instanceof ArrayObject) {
        return "new ArrayObject()";
    }
    return var_export($value, true);
}

function exportProp(object $o, string $p) {
    $rp = new ReflectionProperty($o, $p);

    echo ', ', $p, ': ';
    if ($rp->isInitialized($o)) {
        if ($p === 'readType') {
            echo match($o->$p) {
                0 => 'BP_VAR_R',
                1 => 'BP_VAR_W',
                2 => 'BP_VAR_RW',
                3 => 'BP_VAR_IS',
                4 => 'BP_VAR_FUNC_ARG',
                5 => 'BP_VAR_UNSET',
            };
        } else {
            echo zend_test_var_export($o->$p);
        }
    } else {
        echo 'uninitialized';
    }
}

function exportObject(object $o) {
    echo $o::class;
    foreach (get_class_vars($o::class) as $p => $v) {
        exportProp($o, $p);
    }
    echo "\n";
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
        return 5;
    }
    public function offsetUnset($offset): void {
        var_dump(__METHOD__);
        var_dump($offset);
    }
    public function offsetExists($offset): bool {
        var_dump(__METHOD__);
        var_dump($offset);
        return true;
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
        return 5;
    }
    public function offsetUnset($offset): void {
        var_dump(__METHOD__);
        var_dump($offset);
    }
    public function offsetExists($offset): bool {
        var_dump(__METHOD__);
        var_dump($offset);
        return true;
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
    -12,
    -13.5,
    -14.0,
    //PHP_INT_MAX,
    //PHP_INT_MIN,
    PHP_INT_MAX * 2,
    PHP_INT_MIN * 2,
    INF,
    -INF,
    NAN,
    'string',
    '7',
    '8.5',
    '9.0',
    '2e3',
    '20a',
    '  21',
    '22  ',
    //"9179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368",
    //"-9179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368",
    '0x14',
    '-15',
    '-16.5',
    '-17.0',
    (string) PHP_INT_MAX * 2,
    (string) PHP_INT_MIN * 2,
    [],
    STDERR,
    new stdClass(),
    new ArrayObject(),
    new A(),
    new B(),
];

$failures = [];
$failuresNb = 0;
$testCasesTotal = 0;

$var_dim_filename = __DIR__ . DIRECTORY_SEPARATOR . 'test_variable_offsets.inc';
