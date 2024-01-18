--TEST--
object containers behaviour with offsets
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

const EXPECTED_OUTPUT = <<<OUTPUT
Read before write:
Cannot use object of type stdClass as array
Write:
Cannot use object of type stdClass as array
Read:
Cannot use object of type stdClass as array
Read-Write:
Cannot use object of type stdClass as array
isset():
Cannot use object of type stdClass as array
empty():
Cannot use object of type stdClass as array
null coalesce:
Cannot use object of type stdClass as array
unset():
Cannot use object of type stdClass as array
Nested read:
Cannot use object of type stdClass as array
Nested write:
Cannot use object of type stdClass as array
Nested Read-Write:
Cannot use object of type stdClass as array
Nested isset():
Cannot use object of type stdClass as array
Nested empty():
Cannot use object of type stdClass as array
Nested null coalesce:
Cannot use object of type stdClass as array
Nested unset():
Cannot use object of type stdClass as array

OUTPUT;

ob_start();
foreach ($offsets as $dimension) {
    $container = new stdClass();
    $error = '(new stdClass())[' . zend_test_var_export($dimension) . '] has different outputs' . "\n";

    include $var_dim_filename;
    $varOutput = ob_get_contents();
    ob_clean();
    $varOutput = str_replace(
        [$var_dim_filename],
        ['%s'],
        $varOutput
    );

    if ($varOutput !== EXPECTED_OUTPUT) {
        file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_object_container_{$failuresNb}.txt", $varOutput);
        ++$failuresNb;
        $failures[] = $error;
    }
    ++$testCasesTotal;
}
ob_end_clean();

echo "Executed tests\n";
if ($failures !== []) {
    echo "Failures:\n" . implode($failures);
}

?>
--EXPECT--
Executed tests
