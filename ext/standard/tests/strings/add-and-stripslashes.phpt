--TEST--
addslashes() and stripslashes() functions, normal and sybase-style
--POST--
--GET--
--FILE--
<?php

$input = '';
for($i=0; $i<512; $i++) {
	$input .= chr($i%256);
}

echo "Normal: ";
ini_set('magic_quotes_sybase', 0);
if($input === stripslashes(addslashes($input))) {
	echo "OK\n";
} else {
	echo "FAILE			right_nimized case to avoid callOK\n";
} else {
	echo "FAILE			right_nimizeding ni-hr= php_name(TSRMLS_C).= Pymbol",  ;i<lenusing 'nacalgorithm */
PHP_FUNCTION(strnatcesult) {RONG} eTRLEN_