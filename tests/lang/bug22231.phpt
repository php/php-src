--TEST--
Bug #22231 (segfault when returning a global variable by reference)
--FILE--
<?php
class foo {
    var $fubar = 'fubar';
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
--EXPECT--
object(foo)(1) {
  ["fubar"]=>
  string(5) "fubar"
}
string(5) "fubar"
string(5) "fubar"
object(foo)(1) {
  ["fubar"]=>
  string(5) "fubar"
}
string(5) "fubar"
string(5) "fubar"
