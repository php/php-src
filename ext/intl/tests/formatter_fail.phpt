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

function print_exception($e) {
    echo "\n" . get_class($e) . ": " . $e->getMessage()
       . " in " . $e->getFile() . " on line " . $e->getLine() . "\n";
}

function crt($t, $l, $s) {
    switch(true) {
        case $t == "O":
            try {
                return new NumberFormatter($l, $s);
            } catch (Throwable $e) {
                print_exception($e);
                return null;
            }
            break;
        case $t == "C":
            try {
                return NumberFormatter::create($l, $s);
            } catch (Throwable $e) {
                print_exception($e);
                return null;
            }
            break;
        case $t == "P":
            try {
                return numfmt_create($l, $s);
            } catch (Throwable $e) {
                print_exception($e);
                return null;
            }
            break;
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
} catch (TypeError $e) {
    print_exception($e);
    $fmt = null;
}
err($fmt);
try {
    $fmt = numfmt_create();
} catch (TypeError $e) {
    print_exception($e);
    $fmt = null;
}
err($fmt);
try {
    $fmt = NumberFormatter::create();
} catch (TypeError $e) {
    print_exception($e);
    $fmt = null;
}
err($fmt);

$fmt = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
try {
    $fmt->__construct('en_US', NumberFormatter::DECIMAL);
} catch (Error $e) {
    print_exception($e);
    $fmt = null;
}
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
ArgumentCountError: NumberFormatter::__construct() expects at least 2 arguments, 0 given in %s on line %d
'U_ZERO_ERROR'

ArgumentCountError: numfmt_create() expects at least 2 arguments, 0 given in %s on line %d
'U_ZERO_ERROR'

ArgumentCountError: NumberFormatter::create() expects at least 2 arguments, 0 given in %s on line %d
'U_ZERO_ERROR'

Error: NumberFormatter object is already constructed in %s on line %d
'U_ZERO_ERROR'

IntlException: Constructor failed in %s on line %d
'numfmt_create: number formatter creation failed: U_UNSUPPORTED_ERROR'
'numfmt_create: number formatter creation failed: U_UNSUPPORTED_ERROR'
'numfmt_create: number formatter creation failed: U_UNSUPPORTED_ERROR'

TypeError: NumberFormatter::__construct(): Argument #1 ($locale) must be of type string, array given in %s on line %d
'U_ZERO_ERROR'

TypeError: NumberFormatter::create(): Argument #1 ($locale) must be of type string, array given in %s on line %d
'U_ZERO_ERROR'

TypeError: numfmt_create(): Argument #1 ($locale) must be of type string, array given in %s on line %d
'U_ZERO_ERROR'

IntlException: Constructor failed in %s on line %d
'numfmt_create: number formatter creation failed: U_UNSUPPORTED_ERROR'
'numfmt_create: number formatter creation failed: U_UNSUPPORTED_ERROR'
'numfmt_create: number formatter creation failed: U_UNSUPPORTED_ERROR'

IntlException: Constructor failed in %s on line %d
'numfmt_create: number formatter creation failed: U_MEMORY_ALLOCATION_ERROR'
'numfmt_create: number formatter creation failed: U_MEMORY_ALLOCATION_ERROR'
'numfmt_create: number formatter creation failed: U_MEMORY_ALLOCATION_ERROR'
