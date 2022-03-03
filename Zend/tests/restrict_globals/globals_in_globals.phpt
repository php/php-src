--TEST--
$GLOBALS no longer contains 'GLOBALS'
--FILE--
<?php

$g = $GLOBALS;
var_dump(isset($g['GLOBALS']));

?>
--EXPECT--
bool(false)
