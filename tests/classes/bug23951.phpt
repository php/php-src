--TEST--
Bug #23951: Defines not working in inherited classes
--FILE--
<?php

define('FOO1', 1);
define('FOO2', 2);

class A {
    
    var $a_var = array(FOO1=>'foo1_value', FOO2=>'foo2_value');
    
}

class B extends A {
 
    var $b_var = 'foo';   
            
}

$a = new A;
$b = new B;

print_r($a);
print_r($b);

?>
--EXPECT--
a Object
(
    [a_var] => Array
        (
            [1] => foo1_value
            [2] => foo2_value
        )

)
b Object
(
    [a_var] => Array
        (
            [1] => foo1_value
            [2] => foo2_value
        )

    [b_var] => foo
)
