--TEST--
GET and data callback tests
--GET--
a=1&b=2
--FILE--
<?php
function myfunc($val) {
	return $val . _ . 'callback';
}
echo input_get(INPUT_GET, 'a', FILTER_CALLBACK, 'myfunc');
echo "\n";
echo input_get(INPUT_GET, 'b', FILTER_VALIDATE_INT);
echo "\n";
$data = "data";

echo filter_data($data, FILTER_CALLBACK, 'myfunc');
echo "\n";

$res = input_get_args(INPUT_GET, array(
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
