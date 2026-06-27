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
} catch (Openssl\OpensslException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Failed to import session data
