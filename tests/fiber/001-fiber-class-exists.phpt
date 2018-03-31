--TEST--
tests Fiber class
--FILE--
<?php
echo class_exists('Fiber'), "\n";
$f = new Fiber(function () {});
var_dump($f);
?>
--EXPECTF--
1
object(Fiber)#%d (0) {
}
