--TEST--
GH-9407: LSP error in eval'd code refers to wrong class for static type
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

class A {
    public function duplicate(): static {}
}

class C {
    public static function generate() {
        eval(<<<PHP
            class B extends A {
                public function duplicate(): A {}
            }
        PHP);
    }
}

C::generate();

?>
--EXPECTF--
Fatal error: Declaration of B::duplicate(): A must be compatible with A::duplicate(): static in %s : eval()'d code on line %d
