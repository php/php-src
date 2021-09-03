--TEST--
filter_var() Float exponential weird result
--EXTENSIONS--
filter
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
--EXPECT--
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
