--TEST--
tests Fiber stackful yield value
--FILE--
<?php
function foo()
{
    Fiber::yield(1);
}

$f = new Fiber(function () {
    foo();
    Fiber::yield(2);
});
var_dump($f->resume());
var_dump($f->resume());
?>
--EXPECT--
int(1)
int(2)
