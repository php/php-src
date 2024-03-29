--TEST--
GH-13827 (Null pointer access of type 'zval' in phpdbg_frame)
--FILE--
<?php

$fiber = new Fiber(function () {
    $fiber = Fiber::getCurrent();

    Fiber::suspend();
});

$fiber->start();

$fiber = null;
gc_collect_cycles();

?>
--PHPDBG--
r
t
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Uncaught GracefulExit in  on line 0: ]
>00006:     Fiber::suspend();
 00007: });
 00008: 
prompt> frame #0: {closure}() at %s:6
 => {closure} (internal function)
prompt>
