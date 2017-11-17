--TEST--
Bug #43344.1 (Wrong error message for undefined namespace constant)
--FILE--
<?php
namespace Foo;
function f1($a=bar) {
	return $a;
}
function f2($a=array(bar)) {
	return $a[0];
}
function f3($a=array(bar=>0)) {
	reset($a);
	return key($a);
}
echo bar."\n";
echo f1()."\n";
echo f2()."\n";
echo f3()."\n";
?>
--EXPECTF--
Warning: Use of undefined constant bar - assumed 'bar' (this will throw an Error in a future version of PHP) in %sbug43344_1.php on line 13
bar

Warning: Use of undefined constant bar - assumed 'bar' (this will throw an Error in a future version of PHP) in %sbug43344_1.php on line 3
bar

Warning: Use of undefined constant bar - assumed 'bar' (this will throw an Error in a future version of PHP) in %sbug43344_1.php on line 6
bar

Warning: Use of undefined constant bar - assumed 'bar' (this will throw an Error in a future version of PHP) in %sbug43344_1.php on line 9
bar
