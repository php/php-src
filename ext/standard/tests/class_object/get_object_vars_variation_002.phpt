--TEST--
get_object_vars() - ensure references are preserved
--FILE--
<?php
/* Prototype  : proto array get_object_vars(object obj)
 * Description: Returns an array of object properties 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

$obj = new stdClass;
var_dump(get_object_vars($obj));

$a='original.a';
$obj->ref = &$a;
$obj->val = $a;

$arr = get_object_vars($obj);
var_dump($arr);

$arr['ref'] = 'changed.ref';
$arr['val'] = 'changed.val';

var_dump($arr, $obj, $a);
?>
--EXPECT--
array(0) {
}
array(2) {
  [u"ref"]=>
  &unicode(10) "original.a"
  [u"val"]=>
  unicode(10) "original.a"
}
array(2) {
  [u"ref"]=>
  &unicode(11) "changed.ref"
  [u"val"]=>
  unicode(11) "changed.val"
}
object(stdClass)#1 (2) {
  [u"ref"]=>
  &unicode(11) "changed.ref"
  [u"val"]=>
  unicode(10) "original.a"
}
unicode(11) "changed.ref"
