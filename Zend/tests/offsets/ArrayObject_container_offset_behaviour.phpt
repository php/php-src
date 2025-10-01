--TEST--
ArrayObject containers behaviour with offsets
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

const EXPECTED_OUTPUT_VALID_OFFSETS = <<<OUTPUT
Read before write:

Warning: Undefined array key %s in %s on line %d
NULL
Write:
Read:
int(5)
Read-Write:
isset():
bool(true)
empty():
bool(false)
null coalesce:
int(25)
Reference to dimension:
Value of reference:
int(25)
Value of container dimension after write to reference (should be int(100) if successful):
int(100)
unset():
Nested read:

Warning: Undefined array key %s in %s on line %d

Warning: Trying to access array offset on null in %s on line %d
NULL
Nested write:
Nested Read-Write:
Nested isset():
bool(true)
Nested empty():
bool(false)
Nested null coalesce:
int(30)
Nested unset():
OUTPUT;

$EXPECTED_OUTPUT_VALID_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTED_OUTPUT_VALID_OFFSETS) . '$/s';

const EXPECTF_OUTPUT_FLOAT_OFFSETS = <<<OUTPUT
Read before write:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d

Warning: Undefined array key %s in %s on line %d
NULL
Write:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
Read:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
int(15)
Read-Write:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
isset():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
bool(true)
empty():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
bool(false)
null coalesce:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
int(35)
Reference to dimension:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
Value of reference:
int(35)
Value of container dimension after write to reference (should be int(100) if successful):

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
int(100)
unset():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
Nested read:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d

Warning: Undefined array key 0 in %s on line %d

Warning: Trying to access array offset on null in %s on line %d
NULL
Nested write:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
Nested Read-Write:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
Nested isset():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
bool(true)
Nested empty():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
bool(false)
Nested null coalesce:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d
int(25)
Nested unset():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d

Deprecated: Implicit conversion from float %F to int loses precision in %s on line %d

OUTPUT;

$EXPECTED_OUTPUT_FLOAT_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTF_OUTPUT_FLOAT_OFFSETS) . '$/s';

const EXPECTF_OUTPUT_FLOAT_OOB_OFFSETS = <<<OUTPUT
Read before write:

Warning: The float %F is not representable as an int, cast occurred in %s on line %d

Warning: Undefined array key 0 in %s on line %d
NULL
Write:

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
Read:

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
int(5)
Read-Write:

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
isset():

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
bool(true)
empty():

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
bool(false)
null coalesce:

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
int(25)
Reference to dimension:

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
Value of reference:
int(25)
Value of container dimension after write to reference (should be int(100) if successful):

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
int(100)
unset():

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
Nested read:

Warning: The float %F is not representable as an int, cast occurred in %s on line %d

Warning: Undefined array key 0 in %s on line %d

Warning: Trying to access array offset on null in %s on line 74
NULL
Nested write:

Warning: The float %F is not representable as an int, cast occurred in %s on line %d

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
Nested Read-Write:

Warning: The float %F is not representable as an int, cast occurred in %s on line %d

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
Nested isset():

Warning: The float %F is not representable as an int, cast occurred in %s on line %d

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
bool(true)
Nested empty():

Warning: The float %F is not representable as an int, cast occurred in %s on line %d

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
bool(false)
Nested null coalesce:

Warning: The float %F is not representable as an int, cast occurred in %s on line %d

Warning: The float %F is not representable as an int, cast occurred in %s on line %d
int(30)
Nested unset():

Warning: The float %F is not representable as an int, cast occurred in %s on line %d

Warning: The float %F is not representable as an int, cast occurred in %s on line %d

OUTPUT;

$EXPECTED_OUTPUT_FLOAT_OOB_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTF_OUTPUT_FLOAT_OOB_OFFSETS) . '$/s';

const EXPECTED_OUTPUT_NULL_OFFSETS = <<<OUTPUT
Read before write:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

Warning: Undefined array key "" in %s on line %d
NULL
Write:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
Read:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
int(5)
Read-Write:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
isset():

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
bool(true)
empty():

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
bool(false)
null coalesce:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
int(25)
Reference to dimension:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
Value of reference:
int(25)
Value of container dimension after write to reference (should be int(100) if successful):

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
int(100)
unset():

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
Nested read:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

Warning: Undefined array key "" in %s on line %d

Warning: Trying to access array offset on null in %s on line %d
NULL
Nested write:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
Nested Read-Write:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
Nested isset():

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
bool(true)
Nested empty():

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
bool(false)
Nested null coalesce:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
int(30)
Nested unset():

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

OUTPUT;

$EXPECTED_OUTPUT_NULL_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTED_OUTPUT_NULL_OFFSETS) . '$/s';

const EXPECTED_OUTPUT_INVALID_OFFSETS = <<<OUTPUT
Read before write:
Cannot access offset of type %s on ArrayObject
Write:
Cannot access offset of type %s on ArrayObject
Read:
Cannot access offset of type %s on ArrayObject
Read-Write:
Cannot access offset of type %s on ArrayObject
isset():
Cannot access offset of type %s in isset or empty
empty():
Cannot access offset of type %s in isset or empty
null coalesce:
Cannot access offset of type %s in isset or empty
Reference to dimension:
Cannot access offset of type %s on ArrayObject
unset():
Cannot unset offset of type %s on ArrayObject
Nested read:
Cannot access offset of type %s on ArrayObject
Nested write:
Cannot access offset of type %s on ArrayObject
Nested Read-Write:
Cannot access offset of type %s on ArrayObject
Nested isset():
Cannot access offset of type %s in isset or empty
Nested empty():
Cannot access offset of type %s in isset or empty
Nested null coalesce:
Cannot access offset of type %s in isset or empty
Nested unset():

Notice: Indirect modification of overloaded element of ArrayObject has no effect in %s on line %d
Cannot unset offset of type %s on ArrayObject

OUTPUT;

$EXPECTED_OUTPUT_INVALID_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTED_OUTPUT_INVALID_OFFSETS) . '$/s';

const EXPECTED_OUTPUT_NULL_OFFSET = <<<OUTPUT
Read before write:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 8

Warning: Undefined array key "" in %s on line 8
NULL
Write:
Read:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 22

Warning: Undefined array key "" in %s on line 22
NULL
Read-Write:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 29

Warning: Undefined array key "" in %s on line 29
isset():

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 36
bool(false)
empty():

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 42
bool(true)
null coalesce:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 48
string(7) "default"
Reference to dimension:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 55
Value of reference:
NULL
Value of container dimension after write to reference (should be int(100) if successful):

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 60
int(100)
unset():

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 67
Nested read:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 74

Warning: Undefined array key "" in %s on line 74

Warning: Trying to access array offset on null in %s on line 74
NULL
Nested write:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 81

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 81
Nested Read-Write:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 88

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 88
Nested isset():

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 95

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 95
bool(true)
Nested empty():

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 101

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 101
bool(false)
Nested null coalesce:

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 107

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 107
int(30)
Nested unset():

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line 114

OUTPUT;

const EXPECTED_OUTPUT_RESOURCE_STDERR_OFFSETS = <<<OUTPUT
Read before write:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 8

Warning: Undefined array key 3 in %s on line 8
NULL
Write:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 15
Read:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 22
int(5)
Read-Write:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 29

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 29
isset():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 36
bool(true)
empty():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 42
bool(false)
null coalesce:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 48
int(25)
Reference to dimension:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 55
Value of reference:
int(25)
Value of container dimension after write to reference (should be int(100) if successful):

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 60
int(100)
unset():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 67
Nested read:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 74

Warning: Undefined array key 3 in %s on line 74

Warning: Trying to access array offset on null in %s on line 74
NULL
Nested write:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 81

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 81
Nested Read-Write:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 88

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 88
Nested isset():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 95

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 95
bool(true)
Nested empty():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 101

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 101
bool(false)
Nested null coalesce:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 107

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 107
int(30)
Nested unset():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 114

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 114

OUTPUT;

ob_start();
foreach ($offsets as $dimension) {
    $container = new ArrayObject();
    $error = '(new ArrayObject())[' . zend_test_var_export($dimension) . '] has different outputs' . "\n";
    ob_start();
    var_dump($dimension);
    $var_dump_output = ob_get_clean();

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
        && !preg_match($EXPECTED_OUTPUT_INVALID_OFFSETS_REGEX, $varOutput)
        && !preg_match($EXPECTED_OUTPUT_FLOAT_OFFSETS_REGEX, $varOutput)
        && !preg_match($EXPECTED_OUTPUT_FLOAT_OOB_OFFSETS_REGEX, $varOutput)
        && !preg_match($EXPECTED_OUTPUT_NULL_OFFSETS_REGEX, $varOutput)
        && $varOutput !== EXPECTED_OUTPUT_NULL_OFFSET
        && $varOutput !== EXPECTED_OUTPUT_RESOURCE_STDERR_OFFSETS
    ) {
        file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_ArrayObject_container_{$failuresNb}.txt", $varOutput);
        ++$failuresNb;
        $failures[] = $error;
    }
    ++$testCasesTotal;
}
/* Using offsets as references */
foreach ($offsets as $offset) {
    $dimension = &$offset;
    $container = new ArrayObject();
    $error = '(new ArrayObject())[&' . zend_test_var_export($dimension) . '] has different outputs' . "\n";
    ob_start();
    var_dump($dimension);
    $var_dump_output = ob_get_clean();

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
        && !preg_match($EXPECTED_OUTPUT_INVALID_OFFSETS_REGEX, $varOutput)
        && !preg_match($EXPECTED_OUTPUT_FLOAT_OFFSETS_REGEX, $varOutput)
        && !preg_match($EXPECTED_OUTPUT_FLOAT_OOB_OFFSETS_REGEX, $varOutput)
        && !preg_match($EXPECTED_OUTPUT_NULL_OFFSETS_REGEX, $varOutput)
        && $varOutput !== EXPECTED_OUTPUT_NULL_OFFSET
        && $varOutput !== EXPECTED_OUTPUT_RESOURCE_STDERR_OFFSETS
    ) {
        file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_ArrayObject_container_{$failuresNb}.txt", $varOutput);
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
