--TEST--
OO Bug Test (Bug #7515)
--POST--
--GET--
--FILE--
<?php
class obj {
	function method() {}
    }

function test($o_copy) {
	$o_copy->root->set_in_copied_o=TRUE;
 	var_dump($o_copy);?><BR><?php }

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
echo ($x==$y) ? 'success':'failure'; ?>
--EXPECT--
success
