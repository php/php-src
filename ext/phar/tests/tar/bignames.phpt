--TEST--
Phar: tar with huge filenames
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.tar';
$fname2 = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.2.tar';
$pname = 'phar://' . $fname;

$p1 = new PharData($fname);
$p1[str_repeat('a', 101)] = 'hi';
$p1[str_repeat('a', 255)] = 'hi2';
copy($fname, $fname2);
$p2 = new PharData($fname2);
echo $p2[str_repeat('a', 101)]->getContent() . "\n";
echo $p2[str_repeat('a', 255)]->getContent() . "\n";

try {
	$p2[str_repeat('a', 400)] = 'yuck';
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.tar');
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.2.tar');
?>
--EXPECTF--
hi
hi2
tar-based phar "%sbignames.2.tar" cannot be created, filename "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" is too long for tar file format
===DONE===
