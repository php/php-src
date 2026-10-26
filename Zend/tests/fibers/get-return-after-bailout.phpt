--TEST--
Fiber::getReturn() after bailout
--FILE--
<?php

register_shutdown_function(static function (): void {
    global $fiber;
    var_dump($fiber->getReturn());
});

$fiber = new Fiber(static function (): void {
    var_dump(new class { public function __debugInfo() { return ''; } }); // cause a bailout
});
$fiber->start();

?>
--EXPECTF--
Fatal error: __debugInfo() must return an array in %s on line %d

Fatal error: Uncaught FiberError: Cannot get fiber return value: The fiber exited with a fatal error in %sget-return-after-bailout.php:%d
Stack trace:
#0 %sget-return-after-bailout.php(%d): Fiber->getReturn()
#1 [internal function]: {closure:%s:%d}()
#2 {main}
  thrown in %sget-return-after-bailout.php on line %d
