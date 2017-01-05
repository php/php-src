--TEST--
Static member access
--FILE--
<?php

class A {
    public static $b = 0;
    public static $c = [0, 1];
    public static $A_str = 'A';
}

$A_str = 'A';
$A_obj = new A;
$b_str = 'b';
$c_str = 'c';

var_dump(A::$b);
var_dump($A_str::$b);
var_dump($A_obj::$b);
var_dump(('A' . '')::$b);
var_dump('A'::$b);
var_dump('\A'::$b);
var_dump('A'[0]::$b);
var_dump(A::$$b_str);
var_dump(A::$$c_str[1]);
var_dump(A::$A_str::$b);

?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(1)
int(0)
