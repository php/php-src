--TEST--
GH-16393 (Assertion failure in ext/opcache/jit/zend_jit.c:2897)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1215
opcache.jit_buffer_size=64M
--FILE--
<?php
ini_set('opcache.jit', 'tracing');
class Test {
}
$appendProp2 = (function() {
})->bindTo($test, Test::class);
$appendProp2();
?>
--EXPECTF--
Warning: Undefined variable $test in %sgh16393.php on line 6
