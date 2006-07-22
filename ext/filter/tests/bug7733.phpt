--TEST--
filter_data() Float exponential weird result
--FILE--
<?php
$data = array(
		'E2',
		'10E',
		'2E-',
		'E-2',
		'+E2'
		);
$out = filter_data($data, FILTER_VALIDATE_FLOAT);
var_dump($out);
?>
--EXPECTF--	
array(5) {
  [0]=>
  float(0)
  [1]=>
  float(10)
  [2]=>
  float(2)
  [3]=>
  float(0)
  [4]=>
  float(0)
}
