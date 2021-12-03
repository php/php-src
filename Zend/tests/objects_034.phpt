--TEST--
Array object clobbering by user error handler
--FILE--
<?php
class A {
}

set_error_handler(function () {
    $GLOBALS['a'] = null;
});

$a = new A;
$a[$c] = 'x' ;
var_dump($a);

$a = new A;
$a[$c] .= 'x' ;
var_dump($a);

$a = new A;
$a[$c][$c] = 'x' ;
var_dump($a);
?>
--EXPECT--
NULL
NULL
NULL
