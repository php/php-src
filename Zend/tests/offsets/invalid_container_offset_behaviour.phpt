--TEST--
Invalid containers with offsets
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

$containers = [
    //false,
    true,
    4,
    5.5,
    STDERR,
    //new stdClass(),
];

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
unset():
Cannot unset offset in a non-array variable

OUTPUT;

    foreach ($offsets as $dimension) {
        $error = $containerStr . '[' . zend_test_var_export($dimension) . '] has different outputs' . "\n";

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

echo "Executed tests\n";
if ($failures !== []) {
    echo "Failures:\n" . implode($failures);
}

?>
--EXPECT--
Executed tests
