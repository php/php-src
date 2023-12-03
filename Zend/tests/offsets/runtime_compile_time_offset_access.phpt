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
    $offset_p = makeOffset($offset);
    $container_p = makeContainer($container);
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
    \$container[$offset_p] = 'v';
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
    \$container[$offset_p] .= 'append';
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
    echo "Coalesce():\\n";
    var_dump(\$container[$offset_p] ?? 'default');
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
try {
    echo "unset():\\n";
    unset(\$container[$offset_p]);
} catch (\Throwable \$e) {
    echo \$e->getMessage(), "\\n";
}
test;
    return $fileContent;
}

$const_dim_filename = __DIR__ . DIRECTORY_SEPARATOR . 'compare_binary_offsets_temp.php';

ob_start();
foreach ($containers as $container_orig) {
    foreach ($offsets as $offset) {
        $error = makeContainer($container_orig) . '[' . makeOffset($offset) . '] has different outputs' . "\n";
        file_put_contents($const_dim_filename, makeTestFile($container_orig, $offset));

        include $const_dim_filename;
        $constOutput = ob_get_contents();
        ob_clean();
        $constOutput = str_replace(
            [$const_dim_filename],
            ['%s'],
            $constOutput
        );

        $dimension = $offset;
        $container = $container_orig;
        if (is_object($container_orig)) {
            $container = clone $container_orig;
        }
        include $var_dim_filename;
        $varOutput = ob_get_contents();
        ob_clean();
        $varOutput = str_replace(
            [$var_dim_filename],
            ['%s'],
            $varOutput
        );

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

echo "Executed $testCasesTotal tests\n";
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
Executed 294 tests
