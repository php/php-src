--TEST--
tests Fiber stackful await
--FILE--
<?php
function foo()
{
    echo "before\n";
    await;
    echo "after\n";
}
$f = new Fiber(function () {
    foo();
    await;
    echo "bye\n";
});
$f->resume();
echo "await\n";
$f->resume();
echo "await2\n";
$f->resume();
--EXPECTF--
before
await
after
await2
bye
