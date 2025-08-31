--TEST--
Fibers in destructors 008: Fibers in shutdown sequence
--FILE--
<?php

register_shutdown_function(function () {
    printf("Shutdown\n");
});

class C {
    public static $instance;
    public function __destruct() {
        $f = new Fiber(function () {
            printf("Started\n");
            Fiber::suspend();
            printf("Resumed\n");
            Fiber::suspend();
        });
        $f->start();
        $f->resume();
        // Can not suspend main fiber
        Fiber::suspend();
    }
}

C::$instance = new C();

?>
--EXPECTF--
Shutdown
Started
Resumed

Fatal error: Uncaught FiberError: Cannot suspend outside of a fiber in %s:%d
Stack trace:
#0 %s(%d): Fiber::suspend()
#1 [internal function]: C->__destruct()
#2 {main}
  thrown in %s on line %d
