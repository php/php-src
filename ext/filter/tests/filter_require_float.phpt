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
float(1.234)
float(1.234)
float(1.234)
float(1200)
float(7000)
float(7000)
float(7000)
float(0.007)

custom decimal:
string(31) "Float validation: Invalid float"
float(0.007)
float(1.234)
float(1.234)

Warning: filter_require_var(): decimal separator must be one char in %s on line 34
string(64) "Float validation: Invalid decimal separator. It must be one char"
float(1.234)
string(31) "Float validation: Invalid float"
float(1.234)
