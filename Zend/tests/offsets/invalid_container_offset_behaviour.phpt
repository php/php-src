--TEST--
Invalid containers with offsets
--FILE--
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

$containers = [
    //false,
    true,
    4,
    5.5,
    STDERR,
    //new stdClass(),
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

ob_start();
foreach ($containers as $container) {
    $containerStr = get_zend_debug_type($container);
    $EXPECTED_OUTPUT = <<<OUTPUT
Read before write:

Warning: Trying to access array offset on $containerStr in %s on line 8
NULL
Write:
Cannot use a scalar value as an array
Read:

Warning: Trying to access array offset on $containerStr in %s on line 22
NULL
Read-Write:
Cannot use a scalar value as an array
isset():
bool(false)
empty():
bool(true)
Coalesce():
string(7) "default"

OUTPUT;

    foreach ($offsets as $dimension) {
        $error = $containerStr . '[' . makeOffset($dimension) . '] has different outputs' . "\n";

        include $var_dim_filename;
        $varOutput = ob_get_contents();
        ob_clean();
        $varOutput = str_replace(
            [$var_dim_filename],
            ['%s'],
            $varOutput
        );

        if ($EXPECTED_OUTPUT !== $varOutput) {
            file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_invalid_container_{$failuresNb}.txt", $varOutput);
            ++$failuresNb;
            $failures[] = $error;
        }
        ++$testCasesTotal;
    }

}
ob_end_clean();

echo "Executed $testCasesTotal tests\n";
if ($failures !== []) {
    echo "Failures:\n" . implode($failures);
}

?>
--EXPECT--
Executed 84 tests
