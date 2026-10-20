--TEST--
extract() with negative keys
--FILE--
<?php
$a = [-5 => 'hello', 'world', 2 => 'positive', 'check' => 'extracted'];

function foo(array $a) {
	extract($a, EXTR_PREFIX_ALL, 'prefix');
	var_dump(get_defined_vars());
}

foo($a);
?>
--EXPECT--
array(3) {
  ["a"]=>
  array(4) {
    [-5]=>
    string(5) "hello"
    [-4]=>
    string(5) "world"
    [2]=>
    string(8) "positive"
    ["check"]=>
    string(9) "extracted"
  }
  ["prefix_2"]=>
  string(8) "positive"
  ["prefix_check"]=>
  string(9) "extracted"
}
