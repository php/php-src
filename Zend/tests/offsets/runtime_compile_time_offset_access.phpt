--TEST--
Test binary operands exposing the same behavior at compile as at run time
--INI--
memory_limit=256M
opcache.file_update_protection=1
--SKIPIF--
<?php
//if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--FILE--
<?php

require_once __DIR__ . DIRECTORY_SEPARATOR . 'test_offset_helpers.inc';

function makeTestFile($container, $offset) {
    $offset_p = zend_test_var_export($offset);
    $container_p = zend_test_var_export($container);
    $fileContent = <<<test
<?php

\$container = $container_p;

// Read before write
try {
    echo "Read before write:\\n";
    var_dump(\$container[$offset_p]);
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Write
try {
    echo "Write:\\n";
    \$container[$offset_p] = 5;
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Read
try {
    echo "Read:\\n";
    var_dump(\$container[$offset_p]);
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Read-Write
try {
    echo "Read-Write:\\n";
    \$container[$offset_p] += 20;
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Is
try {
    echo "isset():\\n";
    var_dump(isset(\$container[$offset_p]));
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
try {
    echo "empty():\\n";
    var_dump(empty(\$container[$offset_p]));
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
try {
    echo "null coalesce:\\n";
    var_dump(\$container[$offset_p] ?? 'default');
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Unset
try {
    echo "unset():\\n";
    unset(\$container[$offset_p]);
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Nested read
try {
    echo "Nested read:\\n";
    var_dump(\$container[$offset_p][$offset_p]);
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Nested write
try {
    echo "Nested write:\\n";
    \$container[$offset_p][$offset_p] = 5;
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Nested Read-Write
try {
    echo "Nested Read-Write:\\n";
    \$container[$offset_p][$offset_p] += 25;
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Nested Is
try {
    echo "Nested isset():\\n";
    var_dump(isset(\$container[$offset_p][$offset_p]));
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
try {
    echo "Nested empty():\\n";
    var_dump(empty(\$container[$offset_p][$offset_p]));
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
try {
    echo "Nested null coalesce:\\n";
    var_dump(\$container[$offset_p][$offset_p] ?? 'default');
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
// Nested unset
try {
    echo "Nested unset():\\n";
    unset(\$container[$offset_p][$offset_p]);
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
test;
    return $fileContent;
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
