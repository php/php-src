--TEST--
Bug #79784: Use after free if changing array during undef var during array write fetch
--FILE--
<?php
set_error_handler(function () {
    $GLOBALS['a'] = null;
});

$a[$c] = 'x' ;
var_dump($a);
$a[$c] .= 'x' ;
var_dump($a);
$a[$c][$c] = 'x' ;
var_dump($a);

?>
--EXPECT--
NULL
NULL
NULL
