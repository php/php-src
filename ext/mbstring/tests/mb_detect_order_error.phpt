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

// Call with 2 parameters
$r = mb_detect_order('auto', 'forbidden_second_argument');

?>

--EXPECTF--
Warning: mb_detect_order() expects at most 1 parameter, 2 given in %s on line %d