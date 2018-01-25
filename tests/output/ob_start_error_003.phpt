--TEST--
Test ob_start() with object supplied but no method.
--FILE--
<?php
/*
 * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
 * Function is implemented in main/output.c
*/

Class C {
}

$c = new C;
var_dump(ob_start(array($c)));
echo "done"
?>
--EXPECTF--
Warning: ob_start(): array must have exactly two members in %s on line %d

Notice: ob_start(): failed to create buffer in %s on line 11
bool(false)
done
