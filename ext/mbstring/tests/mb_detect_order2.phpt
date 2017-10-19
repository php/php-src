--TEST--
mb_detect_order()  
--CREDITS--
PHPTestFest 2017 
Bertrand Drouhard <bertrand@drouhard.com>
phpdublin
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
mbstring.language=Japanese
--FILE--
<?php
//$debug = true;
ini_set('include_path', dirname(__FILE__));

// Call with extra double quotes
$r = mb_detect_order('"auto"');
($r === TRUE) ? print "OK_AUTO_DOUBLE_QUOTES\n" : print "NG_AUTO_DOUBLE_QUOTES\n";

?>

--EXPECTF--
OK_AUTO_DOUBLE_QUOTES
