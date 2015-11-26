--TEST--
Phar: include and parser error
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php

$pharconfig = 3;

require_once 'files/phar_oo_test.inc';

Phar::loadPhar($fname);

$pname = 'phar://' . $fname . '/a.php';

var_dump(file_get_contents($pname));
require $pname;

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/files/031.phar.php');
__halt_compiler();
?>
--EXPECTF--
string(25) "<?php echo new new class;"

Parse error: %s in phar://%s031.phar.php/a.php on line %d
