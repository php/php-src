--TEST--
DimensionWriteable containers behaviour with offsets
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

const EXPECTED_OUTPUT = <<<OUTPUT
Read before write:
Cannot read offset of object of type DimensionWrite
Write:
string(25) "DimensionWrite::offsetSet"
VAR_DUMP_OF_OFFSET
int(5)
Read:
Cannot read offset of object of type DimensionWrite
Read-Write:
Cannot read-write offset of object of type DimensionWrite
isset():
Cannot read offset of object of type DimensionWrite
empty():
Cannot read offset of object of type DimensionWrite
null coalesce:
Cannot read offset of object of type DimensionWrite
Reference to dimension:
Cannot fetch offset of object of type DimensionWrite
unset():
Cannot unset offset of object of type DimensionWrite
Nested read:
Cannot read offset of object of type DimensionWrite
Nested write:
Cannot fetch offset of object of type DimensionWrite
Nested Read-Write:
Cannot fetch offset of object of type DimensionWrite
Nested isset():
Cannot read offset of object of type DimensionWrite
Nested empty():
Cannot read offset of object of type DimensionWrite
Nested null coalesce:
Cannot read offset of object of type DimensionWrite
Nested unset():
Cannot fetch offset of object of type DimensionWrite

OUTPUT;

ob_start();
foreach ([DimensionWrite::class] as $class) {
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
