--TEST--
DimensionFetchable containers behaviour with offsets
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

const EXPECTED_OUTPUT = <<<OUTPUT
Read before write:
string(24) "DimensionRead::offsetGet"
VAR_DUMP_OF_OFFSET
int(5)
Write:
Cannot write to offset of object of type DimensionFetch
Read:
string(24) "DimensionRead::offsetGet"
VAR_DUMP_OF_OFFSET
int(5)
Read-Write:
Cannot read-write offset of object of type DimensionFetch
isset():
string(27) "DimensionRead::offsetExists"
VAR_DUMP_OF_OFFSET
bool(true)
empty():
string(27) "DimensionRead::offsetExists"
VAR_DUMP_OF_OFFSET
string(24) "DimensionRead::offsetGet"
VAR_DUMP_OF_OFFSET
bool(false)
null coalesce:
string(27) "DimensionRead::offsetExists"
VAR_DUMP_OF_OFFSET
string(24) "DimensionRead::offsetGet"
VAR_DUMP_OF_OFFSET
int(5)
Reference to dimension:
string(27) "DimensionFetch::offsetFetch"
VAR_DUMP_OF_OFFSET
Value of reference:
object(DimensionFetch)#16 (0) {
}
Value of container dimension after write to reference (should be int(100) if successful):
string(24) "DimensionRead::offsetGet"
VAR_DUMP_OF_OFFSET
int(5)
unset():
Cannot unset offset of object of type DimensionFetch
Nested read:
string(24) "DimensionRead::offsetGet"
VAR_DUMP_OF_OFFSET

Warning: Trying to access array offset on int in %s on line 74
NULL
Nested write:
string(27) "DimensionFetch::offsetFetch"
VAR_DUMP_OF_OFFSET
Cannot write to offset of object of type DimensionFetch
Nested Read-Write:
string(27) "DimensionFetch::offsetFetch"
VAR_DUMP_OF_OFFSET
Cannot read-write offset of object of type DimensionFetch
Nested isset():
string(27) "DimensionRead::offsetExists"
VAR_DUMP_OF_OFFSET
string(24) "DimensionRead::offsetGet"
VAR_DUMP_OF_OFFSET
bool(false)
Nested empty():
string(27) "DimensionRead::offsetExists"
VAR_DUMP_OF_OFFSET
string(24) "DimensionRead::offsetGet"
VAR_DUMP_OF_OFFSET
bool(true)
Nested null coalesce:
string(27) "DimensionRead::offsetExists"
VAR_DUMP_OF_OFFSET
string(24) "DimensionRead::offsetGet"
VAR_DUMP_OF_OFFSET
string(7) "default"
Nested unset():
string(27) "DimensionFetch::offsetFetch"
VAR_DUMP_OF_OFFSET
Cannot unset offset of object of type DimensionFetch

OUTPUT;

ob_start();
foreach ([DimensionFetch::class] as $class) {
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
            ["VAR_DUMP_OF_OFFSET\n", 'CLASS_NAME'],
            [$var_dump_output, $class],
            EXPECTED_OUTPUT
        );
        $varOutput = str_replace(
            '#17',
            '#16',
            $varOutput
        );

        if ($varOutput !== $expected_output) {
            file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_{$class}_container_{$failuresNb}.txt", $varOutput);
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
