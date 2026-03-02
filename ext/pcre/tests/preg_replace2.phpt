--TEST--
preg_replace()
--SKIPIF--
<?php
if (@preg_match('/./u', '') === false) {
    die('skip no utf8 support in PCRE library');
}
?>
--FILE--
<?php

var_dump(preg_replace(array('/\da(.)/ui', '@..@'), '$1', '12Abc'));
var_dump(preg_replace(array('/\da(.)/ui', '@(.)@'), '$1', array('x','a2aA', '1av2Ab')));


var_dump(preg_replace(array('/[\w]+/'), array('$'), array('xyz', 'bdbd')));
var_dump(preg_replace(array('/\s+/', '~[b-d]~'), array('$'), array('x y', 'bd bc')));

?>
--EXPECT--
string(1) "c"
array(3) {
  [0]=>
  string(1) "x"
  [1]=>
  string(2) "aA"
  [2]=>
  string(2) "vb"
}
array(2) {
  [0]=>
  string(1) "$"
  [1]=>
  string(1) "$"
}
array(2) {
  [0]=>
  string(3) "x$y"
  [1]=>
  string(1) "$"
}
