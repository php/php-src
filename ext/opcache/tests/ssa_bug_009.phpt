--TEST--
Incorrect type inference
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class PHP_CodeCoverage
{
    private function addUncoveredFilesFromWhitelist()
    {
        foreach ($uncoveredFiles as $uncoveredFile) {
            for ($i = 1; $i <= $lines; $i++) {
                $data[$uncoveredFile][$i] = PHP_CodeCoverage_Driver::LINE_NOT_EXECUTED;
            }
        }
    }
}
?>
OK
--EXPECT--
OK
