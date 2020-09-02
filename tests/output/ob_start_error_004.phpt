--TEST--
Test ob_start() with non existent callback method.
--FILE--
<?php
/*
 * Function is implemented in main/output.c
*/

Class C {
}

$c = new C;
var_dump(ob_start(array($c, 'f')));
echo "done"
?>
--EXPECTF--
Warning: ob_start(): class C does not have a method "f" in %s on line %d

Notice: ob_start(): Failed to create buffer in %s on line %d
bool(false)
done
