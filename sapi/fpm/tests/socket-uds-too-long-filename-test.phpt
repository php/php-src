--TEST--
FPM: UNIX socket filename is too for test
--SKIPIF--
<?php
include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$socketFilePrefix = __DIR__ . '/socket-file';
$socketFile = sprintf(
    "/this-socket-very-very-very-long-file-fpm-unix-socket-is-too-long-filename-but-starts-anyway%s.sock",
    __DIR__,
    str_repeat('-0000', 11)
);

$cfg = <<<EOT
[global]
error_log = {{FILE:LOG}}

[fpm_pool]
listen = $socketFile
pm = static
pm.max_children = 1
catch_workers_output = yes
EOT;

$tester = new FPM\Tester($cfg);
$tester->testConfig(true, [
    sprintf(
        '/cannot bind to UNIX socket \'%s\' as path is too long '
            . '\(found length: %d, maximal length: \d+\)/',
        preg_quote($socketFile, '/'),
        strlen($socketFile)
    ),
    '/FPM initialization failed/',
]);
?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();
?>
