--TEST--
GET and data callback tests
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--GET--
a=1&b=2
--FILE--
<?php
function myfunc($val) {
	return $val . '_callback';
}
echo filter_input(INPUT_GET, 'a', FILTER_CALLBACK, array("options"=>'myfunc'));
echo "\n";
echo filter_input(INPUT_GET, 'b', FILTER_VALIDATE_INT);
echo "\n";
$data = "data";

echo filter_var($data, FILTER_CALLBACK, array("options"=>'myfunc'));
echo "\n";

$res = filter_input_array(INPUT_GET, array(
				'a' => array(
					'filter' => FILTER_CALLBACK,
					'options' => 'myfunc'
					),
				'b' => FILTER_VALIDATE_INT 
		)
	);

var_dump($res);
?>
--EXPECT--
1_callback
2
data_callback
array(2) {
  ["a"]=>
  string(10) "1_callback"
  ["b"]=>
  int(2)
}
