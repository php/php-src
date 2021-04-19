--TEST--
Test destructor throwing with unfinished fiber
--FILE--
<?php

new class() {
    function __destruct() {
        $fiber = new Fiber(static function() {
            Fiber::suspend();
        });
        $fiber->start();
        throw new Exception;
    }
};

?>
--EXPECTF--
Fatal error: Uncaught Exception in %sthrow-from-destruct-with-fiber.php:%d
Stack trace:
#0 %sthrow-from-destruct-with-fiber.php(%d): class@anonymous->__destruct()
#1 {main}
  thrown in %sthrow-from-destruct-with-fiber.php on line %d
