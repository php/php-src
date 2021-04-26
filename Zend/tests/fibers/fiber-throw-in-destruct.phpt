--TEST--
Fiber throwing from destructor
--FILE--
<?php

$object = new class() {
    public function __destruct()
    {
        $fiber = new Fiber(static function (): int {
            Fiber::suspend(1);
            throw new Exception('test');
        });

        var_dump($fiber->start());
        var_dump($fiber->resume());
    }
};

?>
--EXPECTF--
int(1)

Fatal error: Uncaught Exception: test in %sfiber-throw-in-destruct.php:%d
Stack trace:
#0 [internal function]: class@anonymous::{closure}()
#1 %sfiber-throw-in-destruct.php(%d): Fiber->resume()
#2 [internal function]: class@anonymous->__destruct()
#3 {main}
  thrown in %sfiber-throw-in-destruct.php on line %d
