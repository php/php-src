--TEST--
GH-16508: Missing lineno in inheritance errors of delayed early bound classes
--EXTENSIONS--
opcache
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
--INI--
opcache.enable_cli=1
--FILE--
<?php

new Test2;

class Test2 extends Test {}

abstract class Test {
    abstract function foo();
}

?>
--EXPECTF--
Fatal error: Class Test2 contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (Test::foo) in %s on line 5
