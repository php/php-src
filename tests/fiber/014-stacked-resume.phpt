--TEST--
tests Fiber stackful resume value
--FILE--
<?php
function foo()
{
    return Fiber::yield();
}
$f = new Fiber(function () {
    echo foo();
});
$f->resume();
$f->resume('foo');
?>
--EXPECT--
foo
