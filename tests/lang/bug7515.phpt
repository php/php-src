--TEST--
Bug #7515 (weird & invisible referencing of objects)
--FILE--
<?php
class obj {
    function method() {}
}

$o = new stdClass;
$o->root = new obj();

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
--EXPECT--
success
