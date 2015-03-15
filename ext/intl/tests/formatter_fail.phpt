--TEST--
numfmt creation failures
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

function err($fmt) {
	if(!$fmt) {
		echo var_export(intl_get_error_message(), true)."\n";
	}
}

function crt($t, $l, $s) {
	try {
		$fmt = null;
		switch(true) {
			case $t == "O":
				$fmt = new NumberFormatter($l, $s);
				break;
			case $t == "C":
				$fmt = NumberFormatter::create($l, $s);
				break;
			case $t == "P":
				$fmt = numfmt_create($l, $s);
				break;
		}
		err($fmt); 
	}
	catch(IntlException $ie) {
		echo "IE: ".$ie->getMessage().PHP_EOL;
	}
}

$args = array(
	array(null, null),
	array("whatever", 1234567),
	array(array(), array()),
	array("en", -1),
	array("en_US", NumberFormatter::PATTERN_RULEBASED),
);

try {
	$fmt = new NumberFormatter();
}
catch(IntlException $ie) {
	echo $ie->getMessage().PHP_EOL;
}

$fmt = numfmt_create();
err($fmt); 
$fmt = NumberFormatter::create();
err($fmt); 

foreach($args as $arg) {
	$fmt = crt("O", $arg[0], $arg[1]);
	$fmt = crt("C", $arg[0], $arg[1]);
	$fmt = crt("P", $arg[0], $arg[1]);
}

?>
--EXPECTF--
Warning: NumberFormatter::__construct() expects at least 2 parameters, 0 given in %s on line %d
'numfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'

Warning: numfmt_create() expects at least 2 parameters, 0 given in %s on line %d
'numfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'

Warning: NumberFormatter::create() expects at least 2 parameters, 0 given in %s on line %d
'numfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'
'numfmt_create: number formatter creation failed: U_UNSUPPORTED_ERROR'
'numfmt_create: number formatter creation failed: U_UNSUPPORTED_ERROR'
'numfmt_create: number formatter creation failed: U_UNSUPPORTED_ERROR'

Warning: NumberFormatter::__construct() expects parameter 1 to be string, array given in %s on line %d
'numfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'

Warning: NumberFormatter::create() expects parameter 1 to be string, array given in %s on line %d
'numfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'

Warning: numfmt_create() expects parameter 1 to be string, array given in %s on line %d
IE: numfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR
IE: numfmt_create: number formatter creation failed: U_UNSUPPORTED_ERROR
IE: numfmt_create: number formatter creation failed: U_UNSUPPORTED_ERROR
IE: numfmt_create: number formatter creation failed: U_UNSUPPORTED_ERROR
IE: numfmt_create: number formatter creation failed: U_MEMORY_ALLOCATION_ERROR
IE: numfmt_create: number formatter creation failed: U_MEMORY_ALLOCATION_ERROR
IE: numfmt_create: number formatter creation failed: U_MEMORY_ALLOCATION_ERROR
