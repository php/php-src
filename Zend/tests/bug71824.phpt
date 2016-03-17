--TEST--
Bug #71824 (null ptr deref _zval_get_string_func (zend_operators.c:851))
--INI--
error_reporting=0
--FILE--
<?php
$z = unserialize('O:1:"A":0:{}');
var_dump($z->e.=0);
var_dump(++$z->x);
var_dump($z->y++);

$y = array(PHP_INT_MAX => 0);
var_dump($y[] .= 0);
var_dump(++$y[]);
var_dump($y[]++);
?>
--EXPECT--
string(1) "0"
int(1)
int(1)
NULL
NULL
NULL
