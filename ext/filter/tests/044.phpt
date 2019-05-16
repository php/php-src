--TEST--
Integer validation with spaces
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--INI--
precision=14
--FILE--
<?php
$vals = array(
"	
 ",
" ",
" 123",
" 123.01 ",
);

foreach ($vals as $var) {
	var_dump(filter_var($var, FILTER_VALIDATE_FLOAT));
}
?>
--EXPECT--
bool(false)
bool(false)
float(123)
float(123.01)
