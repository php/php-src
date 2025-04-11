<?php
// Do not run on Windows
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die("skip not for Windows");
}
// Run only if base functions are available
if (!function_exists('dns_get_record')) {
    die("skip DNS functions not available");
}
// Run only if BIND server is running
if (!file_exists(__DIR__ . '/zones/named.pid')) {
    die("skip BIND server is not running");
}
