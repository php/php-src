--TEST--
GH-14643: Segfault on empty user function.
--SKIPIF--
<?php
$tracing = extension_loaded("Zend OPcache")
    && ($conf = opcache_get_configuration()["directives"])
    && array_key_exists("opcache.jit", $conf)
    &&  $conf["opcache.jit"] === "tracing";
if (PHP_OS_FAMILY === "Windows" && PHP_INT_SIZE == 8 && $tracing && getenv('SKIP_ASAN')) {
    $url = "https://github.com/php/php-src/issues/15709";
    die("xfail Test fails on Windows x64 (VS17) and tracing JIT with ASan; see $url");
}
?>
--FILE--
<?php
class Logger {
	public function __construct() {
		register_shutdown_function(function () {
			$this->flush();
			register_shutdown_function([$this, 'flush'], true);
		});
	}
	public function flush($final = false) {
	}
}
while (true) {
	$a = new Logger();
}
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s

Fatal error: Allowed memory size of %d bytes exhausted %s
