--TEST--
mb_substitute_character()  
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
//$debug = true;
ini_set('include_path','.');
include_once('common.inc');

// Note: It does not return TRUE/FALSE for setting char

// Use Unicode val
$r = mb_substitute_character(0x3013);
//$r = mb_substitute_character('U+3013');
($r === TRUE) ?    print "OK_UTF\n" :  print("NG_UTF: ".gettype($r)." $r\n");
print mb_substitute_character() . "\n";


// Use "long"
$r = mb_substitute_character('long');
($r === TRUE) ? print "OK_LONG\n" : print("NG_LONG: ".gettype($r)." $r\n");
print mb_substitute_character() . "\n";


// Use "none"
$r = mb_substitute_character('none');
($r === TRUE) ? print "OK_NONE\n" : print("NG_NONE: ".gettype($r)." $r\n");
print mb_substitute_character() . "\n";


// Set invalid string. Should fail.
print "== INVALID PARAMETER ==\n";
$r = mb_substitute_character('BAD_NAME');
($r === FALSE) ? print "OK_BAD_NAME\n" : print("NG_BAD_NAME: ".gettype($r)." $r\n");

?>

--EXPECT--
OK_UTF
12307
OK_LONG
long
OK_NONE
none
== INVALID PARAMETER ==
ERR: Warning
OK_BAD_NAME

