--TEST--
Test binary operands exposing the same behavior at compile as at run time
--INI--
memory_limit=256M
opcache.file_update_protection=1
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

function makeTestFile($container, $offset) {
    $offset_p = zend_test_var_export($offset);
    $container_p = zend_test_var_export($container);
    $fileContent = file_get_contents(__DIR__ . '/test_variable_offsets.inc');
    $fileContent = str_replace('$dimension', $offset_p, $fileContent);
    return str_replace('//$container var declaration in const generated file', "\$container = $container_p;", $fileContent);
}

function normalize_output(string $output, string $filename): string {
    $output = str_replace(
        [$filename],
        ['%s'],
        $output
    );

    $output = preg_replace('/\)#\d+ \(/', ')#99 (', $output);

    return $output;
}

$const_dim_filename = __DIR__ . DIRECTORY_SEPARATOR . 'compare_binary_offsets_temp.php';

ob_start();
foreach ($containers as $container_orig) {
    foreach ($offsets as $offset) {
        $error = zend_test_var_export($container_orig) . '[' . zend_test_var_export($offset) . '] has different outputs' . "\n";
        file_put_contents($const_dim_filename, makeTestFile($container_orig, $offset));

        include $const_dim_filename;
        $constOutput = ob_get_contents();
        ob_clean();
        $constOutput = normalize_output($constOutput, $const_dim_filename);

        $dimension = $offset;
        $container = $container_orig;
        if (is_object($container_orig)) {
            $container = clone $container_orig;
        }
        include $var_dim_filename;
        $varOutput = ob_get_contents();
        ob_clean();
        $varOutput = normalize_output($varOutput, $var_dim_filename);

        if ($constOutput !== $varOutput) {
            file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_{$failuresNb}_const.txt", $constOutput);
            file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_{$failuresNb}_var.txt", $varOutput);
            file_put_contents(__DIR__ . DIRECTORY_SEPARATOR . "debug_{$failuresNb}_test_case.txt", makeTestFile($container_orig, $offset));
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
--CLEAN--
<?php
$fl = __DIR__ . DIRECTORY_SEPARATOR . 'compare_binary_offsets_temp.php';
@unlink($fl);
?>
--EXPECT--
Executed tests
