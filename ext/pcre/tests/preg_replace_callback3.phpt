--TEST--
preg_replace_callback() 3
--FILE--
<?php

var_dump(preg_replace_callback());
var_dump(preg_replace_callback(1));
var_dump(preg_replace_callback(1,2));
var_dump(preg_replace_callback(1,2,3));
var_dump(preg_replace_callback(1,2,3,4));
$a = 5;
var_dump(preg_replace_callback(1,2,3,4,$a));
$a = "";
var_dump(preg_replace_callback("","","","",$a));
$a = array();
var_dump(preg_replace_callback($a,$a,$a,$a,$a));

echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for preg_replace_callback() in %s on line %d
NULL

Warning: Wrong parameter count for preg_replace_callback() in %s on line %d
NULL

Warning: Wrong parameter count for preg_replace_callback() in %s on line %d
NULL

Warning: preg_replace_callback(): Requires argument 2, '2', to be a valid callback in %s on line %d
int(3)

Warning: preg_replace_callback(): Requires argument 2, '2', to be a valid callback in %s on line %d
int(3)

Warning: preg_replace_callback(): Requires argument 2, '2', to be a valid callback in %s on line %d
int(3)

Warning: preg_replace_callback(): Requires argument 2, '', to be a valid callback in %s on line 1%d
string(0) ""

Warning: preg_replace_callback(): Requires argument 2, 'Array', to be a valid callback in %s on line %d
array(0) {
}
Done
