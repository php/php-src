--TEST--
Bug #29253 (array_diff with $GLOBALS argument fails) 
--FILE--
<?php
$zz = $GLOBALS;
$gg = 'afad';
var_dump(array_diff_assoc($GLOBALS, $zz));
var_dump($gg);
?>
--EXPECT--
array(0) {
}
string(4) "afad"

