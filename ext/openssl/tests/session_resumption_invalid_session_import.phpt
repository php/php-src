--TEST--
TLS session resumption - invalid session import
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!function_exists("proc_open")) die("skip no proc_open");
?>
--FILE--
<?php

try {
    Openssl\Session::import('invalid');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Openssl\OpensslException: Failed to import session data
