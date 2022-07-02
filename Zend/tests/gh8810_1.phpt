--TEST--
GH-8810: Fix reported line number of multi-line function call
--FILE--
<?php

function foo($bar, $baz) {
    throw new Exception();
}

foo
(
    'bar',
    'baz',
);

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:4
Stack trace:
#0 %s(7): foo('bar', 'baz')
#1 {main}
  thrown in %s on line 4
