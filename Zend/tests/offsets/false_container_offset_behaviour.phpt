--TEST--
false containers behaviour with offsets
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

const EXPECTED_OUTPUT_VALID_OFFSETS = <<<OUTPUT
Read before write:

Warning: Trying to access array offset on false in %s on line 8
NULL
Write:

Deprecated: Automatic conversion of false to array is deprecated in %s on line 15
Read:
string(1) "v"
Read-Write:
isset():
bool(true)
empty():
bool(false)
null coalesce:
string(7) "vappend"
unset():

OUTPUT;

const EXPECTF_OUTPUT_FLOAT_OFFSETS = <<<OUTPUT
Read before write:

Warning: Trying to access array offset on false in %s on line 8
NULL
Write:

Deprecated: Automatic conversion of false to array is deprecated %s on line 15

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
null coalesce:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 48
string(7) "vappend"
unset():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 55

OUTPUT;

$EXPECTED_OUTPUT_FLOAT_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTF_OUTPUT_FLOAT_OFFSETS) . '$/s';

const EXPECTED_OUTPUT_INVALID_OFFSETS = <<<OUTPUT
Read before write:

Warning: Trying to access array offset on false in %s on line 8
NULL
Write:

Deprecated: Automatic conversion of false to array is deprecated %s on line 15
Cannot access offset of type %s on array
Read:
Cannot access offset of type %s on array
Read-Write:
Cannot access offset of type %s on array
isset():
Cannot access offset of type %s in isset or empty
empty():
Cannot access offset of type %s in isset or empty
null coalesce:
Cannot access offset of type %s on array
unset():
Cannot unset offset of type %s on array

OUTPUT;

$EXPECTED_OUTPUT_INVALID_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTED_OUTPUT_INVALID_OFFSETS) . '$/s';

const EXPECTED_OUTPUT_RESOURCE_STDERR_OFFSETS = <<<OUTPUT
Read before write:

Warning: Trying to access array offset on false in %s on line 8
NULL
Write:

Deprecated: Automatic conversion of false to array is deprecated in %s on line 15

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 15
Read:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 22
string(1) "v"
Read-Write:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 29
isset():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 36
bool(true)
empty():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 42
bool(false)
null coalesce:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 48
string(7) "vappend"
unset():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 55

OUTPUT;

ob_start();
foreach ($offsets as $dimension) {
    $container = false;
    $error = 'false[' . zend_test_var_export($dimension) . '] has different outputs' . "\n";

    include $var_dim_filename;
    $varOutput = ob_get_contents();
    ob_clean();
    $varOutput = str_replace(
        [$var_dim_filename],
        ['%s'],
        $varOutput
    );

    if (
        $varOutput !== EXPECTED_OUTPUT_VALID_OFFSETS
        && !preg_match($EXPECTED_OUTPUT_INVALID_OFFSETS_REGEX, $varOutput)
        && !preg_match($EXPECTED_OUTPUT_FLOAT_OFFSETS_REGEX, $varOutput)
        && $varOutput !== EXPECTED_OUTPUT_RESOURCE_STDERR_OFFSETS
    ) {
        file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_false_container_{$failuresNb}.txt", $varOutput);
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
