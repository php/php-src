--TEST--
GH-19653 (Closure named argument unpacking between temporary closures can cause a crash) - eval variation
--CREDITS--
arnaud-lb
--FILE--
<?php

function usage1(Closure $c) {
    $c(a: 1);
}

usage1(eval('return function($a) { var_dump($a); };'));
usage1(eval('return function($b) { var_dump($b); };'));

?>
--EXPECTF--
int(1)

Fatal error: Uncaught Error: Unknown named parameter $a in %s:%d
Stack trace:
#0 %s(%d): usage1(Object(Closure))
#1 {main}
  thrown in %s on line %d
