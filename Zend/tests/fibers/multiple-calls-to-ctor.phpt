--TEST--
Multiple calls to constructor are prevented
--FILE--
<?php

$fiber = new Fiber(function () {
    return 123;
});

$fiber->__construct(function () {
    return 321;
});

?>
--EXPECTF--
Fatal error: Uncaught FiberError: Cannot call constructor twice in %smultiple-calls-to-ctor.php:%d
Stack trace:
#0 %smultiple-calls-to-ctor.php(%d): Fiber->__construct(Object(Closure))
#1 {main}
  thrown in %smultiple-calls-to-ctor.php on line %d
