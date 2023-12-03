--TEST--
String containers behaviour with offsets
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

const EXPECTED_OUTPUT_VALID_OFFSETS = <<<OUTPUT
Read before write:

Warning: Uninitialized string offset %d in %s on line %d
string(0) ""
Write:
Read:
string(1) "v"
Read-Write:
Cannot use assign-op operators with string offsets
isset():
bool(true)
empty():
bool(false)
Coalesce():
string(1) "v"
unset():
Cannot unset string offsets

OUTPUT;

$EXPECTED_OUTPUT_VALID_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTED_OUTPUT_VALID_OFFSETS) . '$/s';

const EXPECTED_OUTPUT_VALID_OFFSETS_OUT_OF_RANGE = <<<OUTPUT
Read before write:

Warning: Uninitialized string offset %i in %s on line 8
string(0) ""
Write:

Warning: Illegal string offset %i in %s on line 15
Read:

Warning: Uninitialized string offset %i in %s on line 22
string(0) ""
Read-Write:
Cannot use assign-op operators with string offsets
isset():
bool(false)
empty():
bool(true)
Coalesce():
string(7) "default"
unset():
Cannot unset string offsets

OUTPUT;

$EXPECTED_OUTPUT_VALID_OFFSETS_OUT_OF_RANGE_REGEX = '/^' . expectf_to_regex(EXPECTED_OUTPUT_VALID_OFFSETS_OUT_OF_RANGE) . '$/s';

const EXPECTED_OUTPUT_STRING_CAST_OFFSETS = <<<OUTPUT
Read before write:

Warning: String offset cast occurred in %s on line 8

Warning: Uninitialized string offset %d in %s on line 8
string(0) ""
Write:

Warning: String offset cast occurred in %s on line 15
Read:

Warning: String offset cast occurred in %s on line 22
string(1) "v"
Read-Write:

Warning: String offset cast occurred in %s on line 29
Cannot use assign-op operators with string offsets
isset():
bool(true)
empty():
bool(false)
Coalesce():
string(1) "v"
unset():
Cannot unset string offsets

OUTPUT;

$EXPECTED_OUTPUT_STRING_CAST_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTED_OUTPUT_STRING_CAST_OFFSETS) . '$/s';

const EXPECTED_OUTPUT_STRING_CAST_OFFSETS_OUT_OF_RANGE = <<<OUTPUT
Read before write:

Warning: String offset cast occurred in %s on line 8

Warning: Uninitialized string offset %i in %s on line 8
string(0) ""
Write:

Warning: String offset cast occurred in %s on line 15

Warning: Illegal string offset %i in %s on line 15
Read:

Warning: String offset cast occurred in %s on line 22

Warning: Uninitialized string offset %i in %s on line 22
string(0) ""
Read-Write:

Warning: String offset cast occurred in %s on line 29
Cannot use assign-op operators with string offsets
isset():
bool(false)
empty():
bool(true)
Coalesce():
string(7) "default"
unset():
Cannot unset string offsets

OUTPUT;

$EXPECTED_OUTPUT_STRING_CAST_OFFSETS_OUT_OF_RANGE_REGEX = '/^' . expectf_to_regex(EXPECTED_OUTPUT_STRING_CAST_OFFSETS_OUT_OF_RANGE) . '$/s';

const EXPECTF_OUTPUT_FLOAT_OFFSETS = <<<OUTPUT
Read before write:

Warning: String offset cast occurred in %s on line 8

Warning: Uninitialized string offset %d in %s on line 8
string(0) ""
Write:

Warning: String offset cast occurred in %s on line 15
Read:

Warning: String offset cast occurred in %s on line 22
string(1) "v"
Read-Write:

Warning: String offset cast occurred in %s on line 29
Cannot use assign-op operators with string offsets
isset():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 36
bool(true)
empty():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 42
bool(false)
Coalesce():
string(1) "v"
unset():
Cannot unset string offsets

OUTPUT;

$EXPECTED_OUTPUT_FLOAT_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTF_OUTPUT_FLOAT_OFFSETS) . '$/s';

const EXPECTF_OUTPUT_FLOAT_OFFSETS_OUT_OF_RANGE = <<<OUTPUT
Read before write:

Warning: String offset cast occurred in %s on line 8

Warning: Uninitialized string offset %i in %s on line 8
string(0) ""
Write:

Warning: String offset cast occurred in %s on line 15

Warning: Illegal string offset %i in %s on line 15
Read:

Warning: String offset cast occurred in %s on line 22

Warning: Uninitialized string offset %i in %s on line 22
string(0) ""
Read-Write:

Warning: String offset cast occurred in %s on line 29
Cannot use assign-op operators with string offsets
isset():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 36
bool(false)
empty():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 42
bool(true)
Coalesce():
string(7) "default"
unset():
Cannot unset string offsets

OUTPUT;

$EXPECTF_OUTPUT_FLOAT_OFFSETS_OUT_OF_RANGE_REGEX = '/^' . expectf_to_regex(EXPECTF_OUTPUT_FLOAT_OFFSETS_OUT_OF_RANGE) . '$/s';

const EXPECTED_OUTPUT_INVALID_OFFSETS = <<<OUTPUT
Read before write:
Cannot access offset of type %s on string
Write:
Cannot access offset of type %s on string
Read:
Cannot access offset of type %s on string
Read-Write:
Cannot access offset of type %s on string
isset():
bool(false)
empty():
bool(true)
Coalesce():
Cannot access offset of type %s on string
unset():
Cannot unset string offsets

OUTPUT;

$EXPECTED_OUTPUT_INVALID_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTED_OUTPUT_INVALID_OFFSETS) . '$/s';

const EXPECTED_OUTPUT_INVALID_OFFSETS_AS_STRINGS = <<<OUTPUT
Read before write:
Cannot access offset of type string on string
Write:
Cannot access offset of type string on string
Read:
Cannot access offset of type string on string
Read-Write:
Cannot access offset of type string on string
isset():
bool(false)
empty():
bool(true)
Coalesce():
string(7) "default"
unset():
Cannot unset string offsets

OUTPUT;

const EXPECTED_OUTPUT_INVALID_OFFSETS_AS_LEADING_NUMERIC_STRINGS = <<<OUTPUT
Read before write:

Warning: Illegal string offset %s in %s on line 8

Warning: Uninitialized string offset %d in %s on line 8
string(0) ""
Write:

Warning: Illegal string offset %s in %s on line 15
Read:

Warning: Illegal string offset %s in %s on line 22
string(1) "v"
Read-Write:

Warning: Illegal string offset %s in %s on line 29
Cannot use assign-op operators with string offsets
isset():
bool(false)
empty():
bool(true)
Coalesce():

Warning: Illegal string offset %s in %s on line 48
string(1) "v"
unset():
Cannot unset string offsets

OUTPUT;

$EXPECTED_OUTPUT_INVALID_OFFSETS_AS_LEADING_NUMERIC_STRINGS_REGEX = '/^' . expectf_to_regex(EXPECTED_OUTPUT_INVALID_OFFSETS_AS_LEADING_NUMERIC_STRINGS) . '$/s';

ob_start();
foreach ($offsets as $dimension) {
    $container = '';
    $error = '""[' . makeOffset($dimension) . '] has different outputs' . "\n";

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
        && !preg_match($EXPECTED_OUTPUT_VALID_OFFSETS_OUT_OF_RANGE_REGEX, $varOutput)
        && !preg_match($EXPECTED_OUTPUT_STRING_CAST_OFFSETS_REGEX, $varOutput)
        && !preg_match($EXPECTED_OUTPUT_STRING_CAST_OFFSETS_OUT_OF_RANGE_REGEX, $varOutput)
        && !preg_match($EXPECTED_OUTPUT_FLOAT_OFFSETS_REGEX, $varOutput)
        && !preg_match($EXPECTF_OUTPUT_FLOAT_OFFSETS_OUT_OF_RANGE_REGEX, $varOutput)
        && !preg_match($EXPECTED_OUTPUT_INVALID_OFFSETS_REGEX, $varOutput)
        && $varOutput !== EXPECTED_OUTPUT_INVALID_OFFSETS_AS_STRINGS
        && !preg_match($EXPECTED_OUTPUT_INVALID_OFFSETS_AS_LEADING_NUMERIC_STRINGS_REGEX, $varOutput)
    ) {
        file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_string_container_{$failuresNb}.txt", $varOutput);
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
