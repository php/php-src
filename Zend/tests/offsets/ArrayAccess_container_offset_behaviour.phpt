--TEST--
ArrayAccess containers behaviour with offsets
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

const EXPECTED_OUTPUT = <<<OUTPUT
Read before write:
string(12) "A::offsetGet"
VAR_DUMP_OF_OFFSET
int(5)
Write:
string(12) "A::offsetSet"
VAR_DUMP_OF_OFFSET
int(5)
Read:
string(12) "A::offsetGet"
VAR_DUMP_OF_OFFSET
int(5)
Read-Write:
string(12) "A::offsetGet"
VAR_DUMP_OF_OFFSET
string(12) "A::offsetSet"
VAR_DUMP_OF_OFFSET
int(25)
isset():
string(15) "A::offsetExists"
VAR_DUMP_OF_OFFSET
bool(true)
empty():
string(15) "A::offsetExists"
VAR_DUMP_OF_OFFSET
string(12) "A::offsetGet"
VAR_DUMP_OF_OFFSET
bool(false)
null coalesce:
string(15) "A::offsetExists"
VAR_DUMP_OF_OFFSET
string(12) "A::offsetGet"
VAR_DUMP_OF_OFFSET
int(5)
unset():
string(14) "A::offsetUnset"
VAR_DUMP_OF_OFFSET
Nested read:
string(12) "A::offsetGet"
VAR_DUMP_OF_OFFSET

Warning: Trying to access array offset on int in %s on line 62
NULL
Nested write:
string(12) "A::offsetGet"
VAR_DUMP_OF_OFFSET

Notice: Indirect modification of overloaded element of A has no effect in %s on line 69
Cannot use a scalar value as an array
Nested Read-Write:
string(12) "A::offsetGet"
VAR_DUMP_OF_OFFSET

Notice: Indirect modification of overloaded element of A has no effect in %s on line 76
Cannot use a scalar value as an array
Nested isset():
string(15) "A::offsetExists"
VAR_DUMP_OF_OFFSET
string(12) "A::offsetGet"
VAR_DUMP_OF_OFFSET
bool(false)
Nested empty():
string(15) "A::offsetExists"
VAR_DUMP_OF_OFFSET
string(12) "A::offsetGet"
VAR_DUMP_OF_OFFSET
bool(true)
Nested null coalesce:
string(15) "A::offsetExists"
VAR_DUMP_OF_OFFSET
string(12) "A::offsetGet"
VAR_DUMP_OF_OFFSET
string(7) "default"
Nested unset():
string(12) "A::offsetGet"
VAR_DUMP_OF_OFFSET

Notice: Indirect modification of overloaded element of A has no effect in %s on line 102
Cannot unset offset in a non-array variable

OUTPUT;

ob_start();
foreach ($offsets as $dimension) {
    $container = new A();
    $error = '(new A())[' . zend_test_var_export($dimension) . '] has different outputs' . "\n";
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

    $expected_output = str_replace("VAR_DUMP_OF_OFFSET\n", $var_dump_output, EXPECTED_OUTPUT);

    if ($varOutput !== $expected_output) {
        file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_ArrayAccess_container_{$failuresNb}.txt", $varOutput);
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
