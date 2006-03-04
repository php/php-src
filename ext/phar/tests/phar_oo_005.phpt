--TEST--
Phar and RecursiveDirectoryIterator
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php

require_once 'phar_oo_test.inc';

$it = new RecursiveDirectoryIterator('phar://'.$fname);
$it = new RecursiveIteratorIterator($it);

foreach($it as $name => $ent)
{
	var_dump(str_replace($fname, '*', $name));
	var_dump(str_replace($fname, '*', $ent->getPathname()));
	var_dump($it->getSubPath());
	$sub = $it->getPathInfo();
	var_dump($sub->getFilename());
}

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/phar_oo_test.phar.php');
__halt_compiler();
?>
--EXPECT--
string(14) "phar://*/a.php"
string(14) "phar://*/a.php"
string(0) ""
string(21) "phar_oo_test.phar.php"
string(16) "phar://*/b/c.php"
string(16) "phar://*/b/c.php"
string(1) "b"
string(1) "b"
string(16) "phar://*/b/d.php"
string(16) "phar://*/b/d.php"
string(1) "b"
string(1) "b"
string(14) "phar://*/b.php"
string(14) "phar://*/b.php"
string(0) ""
string(21) "phar_oo_test.phar.php"
string(14) "phar://*/e.php"
string(14) "phar://*/e.php"
string(0) ""
string(21) "phar_oo_test.phar.php"
===DONE===
