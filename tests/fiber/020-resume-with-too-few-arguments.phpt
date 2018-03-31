--TEST--
test too few arguments to resume
--FILE--
<?php
$f = new Fiber(function ($a) {
    Fiber::yield($a);
});
$f->resume();
?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: Too few arguments for callable given to Fiber, 1 required, 0 given in %s020-resume-with-too-few-arguments.php:5
Stack trace:
#0 %s020-resume-with-too-few-arguments.php(5): Fiber->resume()
#1 {main}
  thrown in %s020-resume-with-too-few-arguments.php on line 5
