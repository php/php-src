--TEST--
Test wrong number of arguments and wrong arg types for ob_start()
--FILE--
<?php
/*
 * Function is implemented in main/output.c
*/

Class C {
    static function f($str) {
        return $str;
    }
}

var_dump(ob_start(array("nonExistent","f")));
var_dump(ob_start(array("C","nonExistent")));
var_dump(ob_start("C::no"));
var_dump(ob_start("no"));
echo "done"
?>
--EXPECTF--
Warning: ob_start(): class "nonExistent" not found in %s on line %d

Notice: ob_start(): Failed to create buffer in %s on line %d
bool(false)

Warning: ob_start(): class C does not have a method "nonExistent" in %s on line %d

Notice: ob_start(): Failed to create buffer in %s on line 13
bool(false)

Warning: ob_start(): class C does not have a method "no" in %s on line %d

Notice: ob_start(): Failed to create buffer in %s on line 14
bool(false)

Warning: ob_start(): function "no" not found or invalid function name in %s on line %d

Notice: ob_start(): Failed to create buffer in %s on line 15
bool(false)
done
