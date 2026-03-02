--TEST--
Test return type and value for ob_start()
--FILE--
<?php
/*
 * Function is implemented in main/output.c
*/

var_dump(ob_start());

?>
--EXPECT--
bool(true)
