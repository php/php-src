--TEST--
Phar and RecursiveDirectoryIterator
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (!extension_loaded("spl")) die("skip SPL not available");
if (version_compare(PHP_VERSION, "6.0", "<")) die("skip requires Unicode support");
?>
--INI--
phar.require_hash=0
--FILE--
<?php

require_once 'files/phar_oo_test.inc';
$fname = str_replace('\\', '/', $fname);

$it = new RecursiveDirectoryIterator('phar://'.$fname);
$it = new RecursiveIteratorIterator($it);

foreach($it as $name => $ent)
{
	var_dump(str_replace(array('\\', $fname), array('/', '*'), $name));
	var_dump(str_replace(array('\\', $fname), array('/', '*'), $ent->getPathname()));
	var_dump(str_replace('\\', '/', $it->getSubPath()));
	var_dump(str_replace('\\', '/', $it->getSubPathName()));
	$sub = $it->getPathInfo();
	var_dump(str_replace('\\', '/', $sub->getFilename()));
}

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/files/phar_oo_test.phar.php');
__halt_compiler();
?>
--EXPECT--
unicode(14) "phar://*/a.php"
unicode(14) "phar://*/a.php"
unicode(0) ""
unicode(5) "a.php"
unicode(5) "a.php"
unicode(16) "phar://*/b/c.php"
unicode(16) "phar://*/b/c.php"
unicode(1) "b"
unicode(7) "b/c.php"
unicode(5) "c.php"
unicode(16) "phar://*/b/d.php"
unicode(16) "phar://*/b/d.php"
unicode(1) "b"
unicode(7) "b/d.php"
unicode(5) "d.php"
unicode(14) "phar://*/b.php"
unicode(14) "phar://*/b.php"
unicode(0) ""
unicode(5) "b.php"
unicode(5) "b.php"
unicode(14) "phar://*/e.php"
unicode(14) "phar://*/e.php"
unicode(0) ""
unicode(5) "e.php"
unicode(5) "e.php"
===DONE===
