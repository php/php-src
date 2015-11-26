--TEST--
filter_data() Float exponential weird result
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$data = array(
		'E2',
		'10E',
		'2E-',
		'E-2',
		'+E2'
		);
$out = filter_var($data, FILTER_VALIDATE_FLOAT, FILTER_REQUIRE_ARRAY);
var_dump($out);
?>
--EXPECTF--	
array(5) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
  [2]=>
  bool(false)
  [3]=>
  bool(false)
  [4]=>
  bool(false)
}
