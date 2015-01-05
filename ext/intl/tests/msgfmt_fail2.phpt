--TEST--
msgfmt creation failures icu >= 4.8
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
<?php if(version_compare(INTL_ICU_VERSION, '4.8') < 0) print 'skip'; ?>
--FILE--
<?php

function err($fmt) {
	if(!$fmt) {
		echo var_export(intl_get_error_message(), true)."\n";
	}
}

function crt($t, $l, $s) {
	switch(true) {
		case $t == "O":
			return new MessageFormatter($l, $s);
			break;
		case $t == "C":
			return MessageFormatter::create($l, $s);
			break;
		case $t == "P":
			return msgfmt_create($l, $s);
			break;
	}
}

$args = array(
	array(null, null),
	array("whatever", "{0,whatever}"),
	array(array(), array()),
	array("en", "{0,choice}"),
	array("fr", "{0,"),
	array("en_US", "\xD0"),
);

$fmt = new MessageFormatter();
err($fmt); 
$fmt = msgfmt_create();
err($fmt); 
$fmt = MessageFormatter::create();
err($fmt); 
$fmt = new MessageFormatter('en');
err($fmt); 
$fmt = msgfmt_create('en');
err($fmt); 
$fmt = MessageFormatter::create('en');
err($fmt); 

foreach($args as $arg) {
	$fmt = crt("O", $arg[0], $arg[1]);
	err($fmt);
	$fmt = crt("C", $arg[0], $arg[1]);
	err($fmt);
	$fmt = crt("P", $arg[0], $arg[1]);
	err($fmt);
}

?>
--EXPECTF--
Warning: MessageFormatter::__construct() expects exactly 2 parameters, 0 given in %s on line %d
'msgfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'

Warning: msgfmt_create() expects exactly 2 parameters, 0 given in %s on line %d
'msgfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'

Warning: MessageFormatter::create() expects exactly 2 parameters, 0 given in %s on line %d
'msgfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'

Warning: MessageFormatter::__construct() expects exactly 2 parameters, 1 given in %s on line %d
'msgfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'

Warning: msgfmt_create() expects exactly 2 parameters, 1 given in %s on line %d
'msgfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'

Warning: MessageFormatter::create() expects exactly 2 parameters, 1 given in %s on line %d
'msgfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'
'msgfmt_create: message formatter creation failed: U_ILLEGAL_ARGUMENT_ERROR'
'msgfmt_create: message formatter creation failed: U_ILLEGAL_ARGUMENT_ERROR'
'msgfmt_create: message formatter creation failed: U_ILLEGAL_ARGUMENT_ERROR'
'msgfmt_create: message formatter creation failed: U_ILLEGAL_ARGUMENT_ERROR'
'msgfmt_create: message formatter creation failed: U_ILLEGAL_ARGUMENT_ERROR'
'msgfmt_create: message formatter creation failed: U_ILLEGAL_ARGUMENT_ERROR'

Warning: MessageFormatter::__construct() expects parameter 1 to be string, array given in %s on line %d
'msgfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'

Warning: MessageFormatter::create() expects parameter 1 to be string, array given in %s on line %d
'msgfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'

Warning: msgfmt_create() expects parameter 1 to be string, array given in %s on line %d
'msgfmt_create: unable to parse input parameters: U_ILLEGAL_ARGUMENT_ERROR'
'msgfmt_create: message formatter creation failed: U_PATTERN_SYNTAX_ERROR'
'msgfmt_create: message formatter creation failed: U_PATTERN_SYNTAX_ERROR'
'msgfmt_create: message formatter creation failed: U_PATTERN_SYNTAX_ERROR'
'msgfmt_create: message formatter creation failed: U_UNMATCHED_BRACES'
'msgfmt_create: message formatter creation failed: U_UNMATCHED_BRACES'
'msgfmt_create: message formatter creation failed: U_UNMATCHED_BRACES'
'msgfmt_create: error converting pattern to UTF-16: U_INVALID_CHAR_FOUND'
'msgfmt_create: error converting pattern to UTF-16: U_INVALID_CHAR_FOUND'
'msgfmt_create: error converting pattern to UTF-16: U_INVALID_CHAR_FOUND'
