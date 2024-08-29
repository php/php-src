--TEST--
Closure::bindTo leaks with "fake" closure
--FILE--
<?php
function foo(){
    static $y;
}
Closure::fromCallable('foo')->bindTo(new stdClass);
?>
DONE
--EXPECT--
DONE