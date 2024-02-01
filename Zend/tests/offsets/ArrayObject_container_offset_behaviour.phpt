--TEST--
ArrayObject containers behaviour with offsets
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

const EXPECTED_OUTPUT_VALID_OFFSETS = <<<OUTPUT
Read before write:

Warning: Undefined array key %s in %s on line 8
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
unset():
Nested read:

Warning: Undefined array key %s in %s on line 62

Warning: Trying to access array offset on null in %s on line 62
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

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 8

Warning: Undefined array key %s in %s on line 8
NULL
Write:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 15
Read:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 22
int(15)
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
int(35)
unset():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 55
Nested read:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 62

Warning: Undefined array key 0 in %s on line 62

Warning: Trying to access array offset on null in %s on line 62
NULL
Nested write:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 69

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 69
Nested Read-Write:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 76

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 76
Nested isset():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 83

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 83
bool(true)
Nested empty():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 89

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 89
bool(false)
Nested null coalesce:

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 95

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 95
int(25)
Nested unset():

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 102

Deprecated: Implicit conversion from float %F to int loses precision in %s on line 102

OUTPUT;

$EXPECTED_OUTPUT_FLOAT_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTF_OUTPUT_FLOAT_OFFSETS) . '$/s';

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

Notice: Indirect modification of overloaded element of ArrayObject has no effect in %s on line 102
Cannot unset offset of type %s on ArrayObject

OUTPUT;

$EXPECTED_OUTPUT_INVALID_OFFSETS_REGEX = '/^' . expectf_to_regex(EXPECTED_OUTPUT_INVALID_OFFSETS) . '$/s';

const EXPECTED_OUTPUT_NULL_OFFSET = <<<OUTPUT
Read before write:

Warning: Undefined array key "" in %s on line 8
NULL
Write:
Read:

Warning: Undefined array key "" in %s on line 22
NULL
Read-Write:

Warning: Undefined array key "" in %s on line 29
isset():
bool(false)
empty():
bool(true)
null coalesce:
string(7) "default"
unset():
Nested read:

Warning: Undefined array key "" in %s on line 62

Warning: Trying to access array offset on null in %s on line 62
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
unset():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 55
Nested read:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 62

Warning: Undefined array key 3 in %s on line 62

Warning: Trying to access array offset on null in %s on line 62
NULL
Nested write:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 69

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 69
Nested Read-Write:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 76

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 76
Nested isset():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 83

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 83
bool(true)
Nested empty():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 89

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 89
bool(false)
Nested null coalesce:

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 95

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 95
int(30)
Nested unset():

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 102

Warning: Resource ID#3 used as offset, casting to integer (3) in %s on line 102

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
