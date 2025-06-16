--TEST--
Bug #78689: Closure::fromCallable() doesn't handle [Closure, '__invoke']
--FILE--
<?php
$a = [function () { echo "123\n"; }, '__invoke'];
$a();

$b = Closure::fromCallable($a);
$b();
?>
--EXPECT--
123
123
