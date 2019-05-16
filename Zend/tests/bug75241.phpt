--TEST--
Bug #75241 (Null pointer dereference in zend_mm_alloc_small())
--FILE--
<?php
function eh(){}

set_error_handler('eh');

$d->d = &$d + $d->d/=0;
var_dump($d);
?>
--EXPECT--
float(INF)
