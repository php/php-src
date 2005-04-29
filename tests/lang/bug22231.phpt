--TEST--
Bug #22231 (segfault when returning a global variable by reference)
--INI--
error_reporting=4095
--FILE--
<?php
class foo {
    public $fubar = 'fubar';
}

function &foo(){
    $GLOBALS['foo'] = &new foo();
    return $GLOBALS['foo'];
}
$bar = &foo();
var_dump($bar);
var_dump($bar->fubar);
unset($bar);
$bar = &foo();
var_dump($bar->fubar);

$foo = &foo();
var_dump($foo);
var_dump($foo->fubar);
unset($foo);
$foo = &foo();
var_dump($foo->fubar);
?>
--EXPECTF--
Strict Standards: Assigning the return value of new by reference is deprecated in %s on line %d
object(foo)#%d (1) {
  ["fubar"]=>
  string(5) "fubar"
}
string(5) "fubar"
string(5) "fubar"
object(foo)#%d (1) {
  ["fubar"]=>
  string(5) "fubar"
}
string(5) "fubar"
string(5) "fubar"
