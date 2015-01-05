--TEST--
Bug #30074 (EG(uninitialized_zval_ptr) gets set to reference using EXTR_REFS, affecting later values)
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);   // We don't want the notice for $undefined
$result = extract(array('a'=>$undefined), EXTR_REFS); 
var_dump(array($a));
echo "Done\n";
?>
--EXPECT--
array(1) {
  [0]=>
  NULL
}
Done
