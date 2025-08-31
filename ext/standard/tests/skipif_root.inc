<?php

// Skip if being run by root (files are always readable, writeable and executable)
$filename = @tempnam(__DIR__, 'root_check_');
if (!file_exists($filename)) {
    die('WARN Unable to create the "root check" file');
}

$isRoot = fileowner($filename) == 0;

unlink($filename);

if ($isRoot) {
    die('SKIP Cannot be run as root');
}

