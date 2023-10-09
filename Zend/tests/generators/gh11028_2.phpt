--TEST--
GH-11028 (Heap Buffer Overflow in zval_undefined_cv with generators) - original variant
--FILE--
<?php
$c = (function () {
    [
        ...($r = (function () {
            try {
                yield $a => 0;
            } finally {
                return [];
            }
        })()),
    ];
})()[0];
?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d

Fatal error: Uncaught Error: Keys must be of type int|string during array unpacking in %s:%d
Stack trace:
#0 %s(%d): {closure}()
#1 {main}
  thrown in %s on line %d
