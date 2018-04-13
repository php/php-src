--TEST--
tests Fiber::yield
--FILE--
<?php
$f = new Fiber(function () {
    echo "start\n";
    Fiber::yield();
    echo "end\n";
});
$f->resume();
echo "fiber\n";
$f->resume();
?>
--EXPECT--
start
fiber
end
