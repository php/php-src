--TEST--
Fiber::getReturn() after bailout
--SKIPIF--
<?php
$zend_mm_enabled = getenv("USE_ZEND_ALLOC");
if ($zend_mm_enabled === "0") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php

register_shutdown_function(static function (): void {
    global $fiber;
    var_dump($fiber->getReturn());
});

$fiber = new Fiber(static function (): void {
    str_repeat('X', PHP_INT_MAX);
});
$fiber->start();

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) %sget-return-after-bailout.php on line %d

Fatal error: Uncaught FiberError: Cannot get fiber return value: The fiber exited with a fatal error in %sget-return-after-bailout.php:%d
Stack trace:
#0 %sget-return-after-bailout.php(%d): Fiber->getReturn()
#1 [internal function]: {closure:%s:%d}()
#2 {main}
  thrown in %sget-return-after-bailout.php on line %d
