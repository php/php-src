--TEST--
Phar include and parser error
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php

$pharconfig = 3;

require_once 'phar_oo_test.inc';

Phar::loadPhar($fname);

$pname = 'phar://' . $fname . '/a.php';

var_dump(file_get_contents($pname));
require $pname;

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/phar_oo_test.phar.php');
__halt_compiler();
?>
--EXPECTF--
string(25) "<?php echo new new class;"

Parse error: syntax error, unexpected T_NEW, expecting T_STRING or T_VARIABLE or '$' in phar://%sphar_oo_test.phar.php/a.php on line %d
