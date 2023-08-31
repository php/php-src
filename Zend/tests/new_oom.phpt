--TEST--
Test OOM on new of each class
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--FILE--
<?php

$file = __DIR__ . '/new_oom.inc';
$php = PHP_BINARY;

foreach (get_declared_classes() as $class) {
    $output = shell_exec("$php --no-php-ini $file $class 2>&1");
    if ($output && preg_match('(^\nFatal error: Allowed memory size of [0-9]+ bytes exhausted[^\r\n]* \(tried to allocate [0-9]+ bytes\) in [^\r\n]+ on line [0-9]+$)', $output) !== 1) {
        echo "Class $class failed\n";
        echo $output, "\n";
    }
}

?>
===DONE===
--EXPECT--
===DONE===
