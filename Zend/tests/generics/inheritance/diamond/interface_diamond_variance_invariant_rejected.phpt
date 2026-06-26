--TEST--
Diamond + variance: invariant T interface diamond merges via use-site variance
--FILE--
<?php
interface Box<T> {}
interface FlexBox extends Box<int>, Box<string> {}
echo (new ReflectionClass(FlexBox::class))->getMethods() === [] ? "ok\n" : "unexpected\n";
?>
--EXPECT--
ok
