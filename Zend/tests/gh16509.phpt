--TEST--
GH-16509: Incorrect lineno reported for function redeclaration
--SKIPIF--
<?php
$tracing = extension_loaded("Zend OPcache")
    && ($conf = opcache_get_configuration()["directives"])
    && array_key_exists("opcache.jit", $conf)
    &&  $conf["opcache.jit"] === "tracing";
if (PHP_OS_FAMILY === "Windows" && PHP_INT_SIZE == 8 && $tracing) {
    $url = "https://github.com/php/php-src/pull/14919#issuecomment-2259003979";
    die("xfail Test fails on Windows x64 (VS17) and tracing JIT; see $url");
}
?>
--FILE--
<?php

include __DIR__ . '/gh16509.inc';
include __DIR__ . '/gh16509.inc';

?>
--EXPECTF--
Fatal error: Cannot redeclare function test() (previously declared in %sgh16509.inc:3) in %sgh16509.inc on line 3
