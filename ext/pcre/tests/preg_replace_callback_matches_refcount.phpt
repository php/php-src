--TEST--
preg_replace_callback(): capture match array refcount stays low during callback
--FILE--
<?php
var_dump(preg_replace_callback('/(.)(.)(.)/', static function ($matches) {
    debug_zval_dump($matches);
    return '';
}, 'abc'));

var_dump(preg_replace_callback_array(['/(.)(.)(.)/' => static function ($matches) {
    debug_zval_dump($matches);
    return '';
}], 'abc'));
?>
--EXPECTF--
array(4) packed refcount(2){
  [0]=>
  string(3) "abc"%s
  [1]=>
  string(1) "a" interned
  [2]=>
  string(1) "b" interned
  [3]=>
  string(1) "c" interned
}
string(0) ""
array(4) packed refcount(2){
  [0]=>
  string(3) "abc"%s
  [1]=>
  string(1) "a" interned
  [2]=>
  string(1) "b" interned
  [3]=>
  string(1) "c" interned
}
string(0) ""
