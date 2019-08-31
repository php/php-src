--TEST--
preg_replace_callback() 3
--FILE--
<?php

var_dump(preg_replace_callback(1,2,3));
var_dump(preg_replace_callback(1,2,3,4));
$a = 5;
var_dump(preg_replace_callback(1,2,3,4,$a));

echo "Done\n";
?>
--EXPECTF--
Warning: preg_replace_callback(): Requires argument 2, '2', to be a valid callback in %s on line %d
string(1) "3"

Warning: preg_replace_callback(): Requires argument 2, '2', to be a valid callback in %s on line %d
string(1) "3"

Warning: preg_replace_callback(): Requires argument 2, '2', to be a valid callback in %s on line %d
string(1) "3"
Done
