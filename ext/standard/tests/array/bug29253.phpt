--TEST--
Bug #29253 (array_diff with $GLOBALS argument fails)
--FILE--
<?php
$zz = $GLOBALS;
$gg = 'afad';
var_dump(array_diff_assoc($GLOBALS, $zz));
var_dump($gg);
?>
--EXPECTF--
Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
array(0) {
}
string(4) "afad"
--UEXPECTF--
Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
array(0) {
}
unicode(4) "afad"
