--TEST--
\DimensionUnsetable containers behaviour with offsets
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

const EXPECTED_OUTPUT = <<<OUTPUT
Read before write:
Cannot read offset of object of type DimensionUnset
Write:
Cannot write to offset of object of type DimensionUnset
Read:
Cannot read offset of object of type DimensionUnset
Read-Write:
Cannot read-write offset of object of type DimensionUnset
isset():
Cannot read offset of object of type DimensionUnset
empty():
Cannot read offset of object of type DimensionUnset
null coalesce:
Cannot read offset of object of type DimensionUnset
Reference to dimension:
Cannot fetch offset of object of type DimensionUnset
unset():
string(27) "DimensionUnset::offsetUnset"
VAR_DUMP_OF_OFFSET
Nested read:
Cannot read offset of object of type DimensionUnset
Nested write:
Cannot fetch offset of object of type DimensionUnset
Nested Read-Write:
Cannot fetch offset of object of type DimensionUnset
Nested isset():
Cannot read offset of object of type DimensionUnset
Nested empty():
Cannot read offset of object of type DimensionUnset
Nested null coalesce:
Cannot read offset of object of type DimensionUnset
Nested unset():
Cannot fetch offset of object of type DimensionUnset

OUTPUT;

ob_start();
foreach ([DimensionUnset::class] as $class) {
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
