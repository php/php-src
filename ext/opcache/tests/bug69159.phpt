--TEST--
Bug #69159 (Opcache causes problem when passing a variable variable to a function)
--INI--
opcache.enable=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
$i = 1;
$x1 = "okey";
myFunction(${"x$i"});

function myFunction($x) {
    var_dump($x);
}

?>
--EXPECT--
string(4) "okey"
