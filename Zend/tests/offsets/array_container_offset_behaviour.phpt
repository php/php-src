--TEST--
array containers behaviour with offsets
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

const EXPECTED_OUTPUT_VALID_OFFSETS = <<<OUTPUT
Read before write:

Warning: Undefined array key %s in %s on line 8
NULL
Write:
Read:
string(1) "v"
Read-Write:
isset():
bool(true)
empty():
bool(false)
Coalesce():
string(7) "vappend"
unset():

OUTPUT;

$EXPECTED_OUTPUT_VALID_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTED_OUTPUT_VALID_OFFSETS) . '$/s';

const EXPECTF_OUTPUT_FLOAT_OFFSETS = <<<OUTPUT
Read before write:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 8

Warning: Undefined array key %s in %s on line 8
NULL
Write:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 15
Read:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 22
string(1) "v"
Read-Write:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 29
isset():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 36
bool(true)
empty():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 42
bool(false)
Coalesce():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 48
string(7) "vappend"
unset():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 54

OUTPUT;

$EXPECTED_OUTPUT_FLOAT_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTF_OUTPUT_FLOAT_OFFSETS) . '$/s';

const EXPECTED_OUTPUT_INVALID_OFFSETS = <<<OUTPUT
Read before write:
Cannot access offset of type %s on array
Write:
Cannot access offset of type %s on array
Read:
Cannot access offset of type %s on array
Read-Write:
Cannot access offset of type %s on array
isset():
Cannot access offset of type %s in isset or empty
empty():
Cannot access offset of type %s in isset or empty
Coalesce():
Cannot access offset of type %s on array

OUTPUT;

ob_start();
foreach ($offsets as $dimension) {
    $container = [];
    $error = '[][' . makeOffset($dimension) . '] has different outputs' . "\n";

    include $var_dim_filename;
    $varOutput = ob_get_contents();
    ob_clean();
    $varOutput = str_replace(
        [$var_dim_filename],
        ['%s'],
        $varOutput
    );

    if (
        !preg_match($EXPECTED_OUTPUT_VALID_OFFSETS_REGEX, $varOutput)
        && $varOutput !== EXPECTED_OUTPUT_INVALID_OFFSETS
        && !preg_match($EXPECTED_OUTPUT_FLOAT_OFFSETS_REGEX, $varOutput)
    ) {
        file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_array_container_{$failuresNb}.txt", $varOutput);
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
