--TEST--
tests Fiber await
--FILE--
<?php
$f = new Fiber(function () {
    echo "start\n";
    await;
    echo "end\n";
});
$f->resume();
echo "fiber\n";
$f->resume();
--EXPECTF--
start
fiber
end
