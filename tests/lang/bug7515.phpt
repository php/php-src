--TEST--
Bug #7515 (weird & invisible referencing of objects)
--SKIPIF--
<?php if(version_compare(zend_version(), "2.0.0-dev", '<')) echo "skip Zend Engine 2 needed\n"; ?>
--INI--
error_reporting=2039
--FILE--
<?php
class obj {
	function method() {}
}

$o->root=new obj();

ob_start();
var_dump($o);
$x=ob_get_contents();
ob_end_clean();

$o->root->method();

ob_start();
var_dump($o);
$y=ob_get_contents();
ob_end_clean();
if ($x == $y) {
    print "success";
} else {
    print "failure
x=$x
y=$y
";
}
?>
--EXPECTF--
Warning: Creating default object from empty value in %s on line %d
success
