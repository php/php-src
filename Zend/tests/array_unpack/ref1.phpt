--TEST--
Array unpacking with element rc=1
--FILE--
<?php

$a = 1;
$b = [&$a]; //array (0 => (refcount=2, is_ref=1)=1)

unset($a); //array (0 => (refcount=1, is_ref=1)=1)

var_dump([...$b]); //array (0 => (refcount=0, is_ref=0)=1)

?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
