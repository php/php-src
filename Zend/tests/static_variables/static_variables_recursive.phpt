--TEST--
Static variable with recursive initializer
--FILE--
<?php

function foo($i) {
    static $a = $i <= 10 ? foo($i + 1) : "Done $i";
    var_dump($a);
    return $i;
}

foo(0);
foo(5);

?>
--EXPECT--
string(7) "Done 11"
string(7) "Done 11"
string(7) "Done 11"
string(7) "Done 11"
string(7) "Done 11"
string(7) "Done 11"
string(7) "Done 11"
string(7) "Done 11"
string(7) "Done 11"
string(7) "Done 11"
string(7) "Done 11"
string(7) "Done 11"
string(7) "Done 11"
