--TEST--
Bug #26182 (Object properties created redundantly)
--INI--
error_reporting=2039
--FILE--
<?php

class A {
    function NotAConstructor ()
    {
        if (isset($this->x)) {
            //just for demo
        }
    }
}

$t = new A ();

print_r($t);
var_dump(property_exists($t, 'x'));
var_dump(property_exists($t, 'y'));
var_dump(property_exists($t, 'z'));
var_dump(isset($t->y));
var_dump(property_exists($t, 'y'));
$t->z = 1;
var_dump(property_exists($t, 'z'));

?>
--EXPECT--
a Object
(
    [x] => 
)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
