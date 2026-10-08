<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die("skip not for Windows");
}
if (!function_exists('dns_get_record')) {
    die("skip DNS functions not available");
}
if (!file_exists(__DIR__ . '/named.pid')) {
    die("skip BIND test server is not running");
}
$resolv = @file_get_contents('/etc/resolv.conf');
if ($resolv === false || !preg_match('/^\s*nameserver\s+127\.0\.0\.1\s*$/m', $resolv)) {
    die("skip /etc/resolv.conf does not use the BIND test server");
}
