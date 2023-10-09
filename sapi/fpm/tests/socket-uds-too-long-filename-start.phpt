--TEST--
FPM: UNIX socket filename is too for start
--SKIPIF--
<?php
include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";
$socketFilePrefix = __DIR__ . '/socket-file';
$socketFile = sprintf(
    "%s-fpm-unix-socket-too-long-filename-but-starts-anyway%s.sock",
    $socketFilePrefix,
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
$tester->start();
$tester->expectLogStartNotices();
$tester->expectLogPattern(
    sprintf(
        '/\[pool fpm_pool\] cannot bind to UNIX socket \'%s\' as path is too long '
            . '\(found length: %d, maximal length: \d+\), trying cut socket path instead \'.+\'/',
        preg_quote($socketFile, '/'),
        strlen($socketFile)
    ),
    true
);

$files = glob($socketFilePrefix . '*');

if ($files === []) {
    echo 'Socket files were not found.' . PHP_EOL;
}

if ($socketFile === $files[0]) {
    // this means the socket file path length is not an issue (anymore). Might be not long enough
    echo 'Socket file is the same as configured.' . PHP_EOL;
}

$tester->terminate();
$tester->expectLogTerminatingNotices();
$tester->close();
?>
Done
--EXPECT--
Done
--CLEAN--
<?php
require_once "tester.inc";
FPM\Tester::clean();

// cleanup socket file if php-fpm was not killed
$socketFile = sprintf(
    "/socket-file-fpm-unix-socket-too-long-filename-but-starts-anyway%s.sock",
    __DIR__,
    str_repeat('-0000', 11)
);

if (is_file($socketFile)) {
    unlink($socketFile);
}
?>
