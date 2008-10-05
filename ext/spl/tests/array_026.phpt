--TEST--
SPL: ArrayObject indirect offsetGet overwriting EG(uninitialized_zvar_ptr)
--FILE--
<?php
$test = new ArrayObject();
$test['d1']['d2'] = 'hello';
$test['d1']['d3'] = 'world';
var_dump($test, $test3['mmmmm']);
?>
--EXPECTF--
Notice: Undefined variable: test3 in %s%earray_026.php on line 5
object(ArrayObject)#%d (1) {
  [u"storage":u"ArrayObject":private]=>
  array(1) {
    [u"d1"]=>
    array(2) {
      [u"d2"]=>
      unicode(5) "hello"
      [u"d3"]=>
      unicode(5) "world"
    }
  }
}
NULL
