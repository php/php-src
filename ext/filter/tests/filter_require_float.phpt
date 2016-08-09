--TEST--
filter_require_var() and FLOAT
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--INI--
precision=14
--FILE--
<?php

$floats = array(
'1.234   ',
'   1.234',
'1.234'	,
'1.2e3',
'7E3',
'7E3     ',
'  7E3     ',
'  7E-3     '
);

foreach ($floats as $float) {
	try {
		$out = filter_require_var($float, FILTER_VALIDATE_FLOAT);
	} catch (Exception $e) {
		var_dump($e->getMessage());
	}
	var_dump($out);
}

$floats = array(
'1.234   '	=> ',',
'1,234'		=> ',',
'   1.234'	=> '.',
'1.234'		=> '..',
'1.2e3'		=> ','
);

echo "\ncustom decimal:\n";
foreach ($floats as $float => $dec) {
	try {
		$out = filter_require_var($float, FILTER_VALIDATE_FLOAT, array("options"=>array('decimal' => $dec)));
	} catch (Exception $e) {
		var_dump($e->getMessage());
	}
	var_dump($out); // $out is not updated due to Exception
}

?>
--EXPECTF--
string(115) "Float validation: Invalid float format. Found spaces (invalid_key: N/A, filter_name: float, filter_flags: 33554432)"

Notice: Undefined variable: out in %s on line 20
NULL
string(115) "Float validation: Invalid float format. Found spaces (invalid_key: N/A, filter_name: float, filter_flags: 33554432)"

Notice: Undefined variable: out in %s on line 20
NULL
float(1.234)
float(1200)
float(7000)
string(115) "Float validation: Invalid float format. Found spaces (invalid_key: N/A, filter_name: float, filter_flags: 33554432)"
float(7000)
string(115) "Float validation: Invalid float format. Found spaces (invalid_key: N/A, filter_name: float, filter_flags: 33554432)"
float(7000)
string(115) "Float validation: Invalid float format. Found spaces (invalid_key: N/A, filter_name: float, filter_flags: 33554432)"
float(7000)

custom decimal:
string(115) "Float validation: Invalid float format. Found spaces (invalid_key: N/A, filter_name: float, filter_flags: 33554432)"
float(7000)
float(1.234)
string(115) "Float validation: Invalid float format. Found spaces (invalid_key: N/A, filter_name: float, filter_flags: 33554432)"
float(1.234)

Warning: filter_require_var(): decimal separator must be one char in %s on line 34
string(127) "Float validation: Invalid decimal separator. It must be one char (invalid_key: N/A, filter_name: float, filter_flags: 33554432)"
float(1.234)
string(94) "Float validation: Invalid float (invalid_key: N/A, filter_name: float, filter_flags: 33554432)"
float(1.234)
