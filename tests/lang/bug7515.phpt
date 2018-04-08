--TEST--
Bug #7515 (weird & invisible referencing of objects)
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
