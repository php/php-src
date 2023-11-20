--TEST--
Test binary operands exposing the same behavior at compile as at run time
--INI--
memory_limit=256M
opcache.file_update_protection=1
--SKIPIF--
<?php
//if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--FILE--
<?php

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

function makeTestFile($container, $offset) {
    $offset_p = makeOffset($offset);
    $container_p = makeContainer($container);
    $fileContent = <<<test
<?php

\$container = $container_p;

// Read before write
try {
    echo "Read before write:\\n";
    var_dump(\$container[$offset_p]);
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Write
try {
    echo "Write:\\n";
    \$container[$offset_p] = 'v';
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Read
try {
    echo "Read:\\n";
    var_dump(\$container[$offset_p]);
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Read-Write
try {
    echo "Read-Write:\\n";
    \$container[$offset_p] .= 'append';
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Is
try {
    echo "isset():\\n";
    var_dump(isset(\$container[$offset_p]));
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
try {
    echo "empty():\\n";
    var_dump(empty(\$container[$offset_p]));
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
try {
    echo "Coalesce():\\n";
    var_dump(\$container[$offset_p] ?? 'default');
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
test;
    return $fileContent;
}

$const_dim_filename = __DIR__ . DIRECTORY_SEPARATOR . 'compare_binary_offsets_temp.php';

$failures = [];
$failuresNb = 0;
$testCasesTotal = 0;

$var_dim_filename = __DIR__ . DIRECTORY_SEPARATOR . 'test_variable_offsets.inc';

ob_start();
foreach ($containers as $container_orig) {
    foreach ($offsets as $offset) {
        $error = makeContainer($container_orig) . '[' . makeOffset($offset) . '] has different outputs' . "\n";
        file_put_contents($const_dim_filename, makeTestFile($container_orig, $offset));

        include $const_dim_filename;
        $constOutput = ob_get_contents();
        ob_clean();
        $constOutput = str_replace(
            [$const_dim_filename],
            ['%s'],
            $constOutput
        );

        $dimension = $offset;
        $container = $container_orig;
        include $var_dim_filename;
        $varOutput = ob_get_contents();
        ob_clean();
        $varOutput = str_replace(
            [$var_dim_filename],
            ['%s'],
            $varOutput
        );

        if ($constOutput !== $varOutput) {
            file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_const_{$failuresNb}.txt", $constOutput);
            file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_var_{$failuresNb}.txt", $varOutput);
            file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_test_case_{$failuresNb}.txt", makeTestFile($container_orig, $offset));
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
--CLEAN--
<?php
$fl = __DIR__ . DIRECTORY_SEPARATOR . 'compare_binary_offsets_temp.php';
@unlink($fl);
?>
--EXPECT--
Executed 231 tests
