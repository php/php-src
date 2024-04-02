--TEST--
ArrayAccess containers behaviour with offsets
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

const EXPECTED_OUTPUT = <<<OUTPUT
Read before write:
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET
int(5)
Write:
string(12) "CLASS_NAME::offsetSet"
VAR_DUMP_OF_OFFSET
int(5)
Read:
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET
int(5)
Read-Write:
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET
string(12) "CLASS_NAME::offsetSet"
VAR_DUMP_OF_OFFSET
int(25)
isset():
string(15) "CLASS_NAME::offsetExists"
VAR_DUMP_OF_OFFSET
bool(true)
empty():
string(15) "CLASS_NAME::offsetExists"
VAR_DUMP_OF_OFFSET
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET
bool(false)
null coalesce:
string(15) "CLASS_NAME::offsetExists"
VAR_DUMP_OF_OFFSET
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET
int(5)
Reference to dimension:
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET

Notice: Indirect modification of overloaded element of CLASS_NAME has no effect in %s on line 55
Value of reference:
int(5)
Value of container dimension after write to reference (should be int(100) if successful):
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET
int(5)
unset():
string(14) "CLASS_NAME::offsetUnset"
VAR_DUMP_OF_OFFSET
Nested read:
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET

Warning: Trying to access array offset on int in %s on line 74
NULL
Nested write:
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET

Notice: Indirect modification of overloaded element of CLASS_NAME has no effect in %s on line 81
Cannot use a scalar value as an array
Nested Read-Write:
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET

Notice: Indirect modification of overloaded element of CLASS_NAME has no effect in %s on line 88
Cannot use a scalar value as an array
Nested isset():
string(15) "CLASS_NAME::offsetExists"
VAR_DUMP_OF_OFFSET
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET
bool(false)
Nested empty():
string(15) "CLASS_NAME::offsetExists"
VAR_DUMP_OF_OFFSET
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET
bool(true)
Nested null coalesce:
string(15) "CLASS_NAME::offsetExists"
VAR_DUMP_OF_OFFSET
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET
string(7) "default"
Nested unset():
string(12) "CLASS_NAME::offsetGet"
VAR_DUMP_OF_OFFSET

Notice: Indirect modification of overloaded element of CLASS_NAME has no effect in %s on line 114
Cannot unset offset in a non-array variable

OUTPUT;

ob_start();
foreach (['A', 'B'] as $class) {
    foreach ($offsets as $dimension) {
        $container = new $class();
        $error = "(new $class())[" . zend_test_var_export($dimension) . '] has different outputs' . "\n";
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

        $expected_output = str_replace(
            ["VAR_DUMP_OF_OFFSET\n", "CLASS_NAME"],
            [$var_dump_output, $class],
            EXPECTED_OUTPUT
        );

        if ($varOutput !== $expected_output) {
            file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_ArrayAccess_container_{$failuresNb}.txt", $varOutput);
            ++$failuresNb;
            $failures[] = $error;
        }
        ++$testCasesTotal;
    }
    /* Using offsets as references */
    foreach ($offsets as $offset) {
        $dimension = &$offset;
        $container = new $class();
        $error = "(new $class())[&" . zend_test_var_export($dimension) . '] has different outputs' . "\n";
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

        $expected_output = str_replace(
            ["VAR_DUMP_OF_OFFSET\n", "CLASS_NAME"],
            [$var_dump_output, $class],
            EXPECTED_OUTPUT
        );

        if ($varOutput !== $expected_output) {
            file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_ArrayAccess_container_{$failuresNb}.txt", $varOutput);
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
