--TEST--
Test return type and value for ob_start()
--FILE--
<?php
/*
 * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
 * Function is implemented in main/output.c
*/

var_dump(ob_start());

?>
--EXPECT--
bool(true)
