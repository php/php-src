--TEST--
prev - ensure we cannot pass a temporary
--FILE--
<?php
/* Prototype  : mixed prev(array $array_arg)
 * Description: Move array argument's internal pointer to the previous element and return it 
 * Source code: ext/standard/array.c
 */

/*
 * Pass temporary variables to prev() to test behaviour
 */


var_dump(prev(array(1, 2)));
?>
--EXPECTF--

Fatal error: Only variables can be passed by reference in %s on line %d