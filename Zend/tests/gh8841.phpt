--TEST--
GH-8841 (php-cli core dump calling a badly formed function)
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
register_shutdown_function(function() {
    echo "Before calling g()\n";
    g(1);
    echo "After calling g()\n";
});

register_shutdown_function(function() {
    echo "Before calling f()\n";
    f(1);
    echo "After calling f()\n";
});

eval('function g($x): int { return $x; }');
eval('function f($x): void { return $x; }');
?>
--EXPECTF--
Fatal error: A void function must not return a value in %s on line %d
Before calling g()
After calling g()
Before calling f()

Fatal error: Uncaught Error: Call to undefined function f() in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}()
#1 {main}
  thrown in %s on line %d
