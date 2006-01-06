--TEST--
Phar: phar:// require from within
--INI--
magic_quotes_runtime=0
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = "<?php __HALT_COMPILER(); ?>";
$a = '<?php echo "This is a\n"; require "'.$pname.'/b.php"; ?>';
$b = '<?php echo "This is b\n"; require "'.$pname.'/b/c.php"; ?>';
$c = '<?php echo "This is b/c\n"; require "'.$pname.'/b/d.php"; ?>';
$d = '<?php echo "This is b/d\n"; require "'.$pname.'/e.php"; ?>';
$e = '<?php echo "This is e\n"; ?>';

$manifest = '';
$manifest .= pack('V', 5) . 'a.php' .   pack('VVVV', strlen($a), time(),                        0, strlen($a) + 8);
$manifest .= pack('V', 5) . 'b.php' .   pack('VVVV', strlen($b), time(), strlen($a)          +  8, strlen($b) + 8);
$manifest .= pack('V', 7) . 'b/c.php' . pack('VVVV', strlen($c), time(), strlen($a.$b)       + 16, strlen($c) + 8);
$manifest .= pack('V', 7) . 'b/d.php' . pack('VVVV', strlen($d), time(), strlen($a.$b.$c)    + 24, strlen($d) + 8);
$manifest .= pack('V', 5) . 'e.php' .   pack('VVVV', strlen($e), time(), strlen($a.$b.$c.$d) + 32, strlen($e) + 8);
$file .= pack('VV', strlen($manifest) + 4, 5) .
	 $manifest .
	 pack('VV', crc32($a), strlen($a)) . $a .
	 pack('VV', crc32($b), strlen($b)) . $b .
	 pack('VV', crc32($c), strlen($c)) . $c .
	 pack('VV', crc32($d), strlen($d)) . $d .
	 pack('VV', crc32($e), strlen($e)) . $e;

file_put_contents($fname, $file);

require $pname . '/a.php';

?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
This is a
This is b
This is b/c
This is b/d
This is e
===DONE===
