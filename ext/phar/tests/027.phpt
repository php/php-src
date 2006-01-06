--TEST--
Phar: phar:// opendir
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

function dump($phar, $base)
{
	var_dump($phar . $base);
	$dir = opendir($phar . $base);
	if ($base == '/')
	{
		$base = '';
	}
	while (false !== ($entry = readdir($dir))) {
		$entry = $base . '/' . $entry;
		var_dump($entry);
		var_dump(is_dir($phar . $entry));
		if (is_dir($phar . $entry))
		{
			dump($phar, $entry);
		}
	}
}

dump($pname, '/');

?>
===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECTF--
string(52) "phar://%s027.phar.php/"
string(6) "/a.php"
bool(false)
string(2) "/b"
bool(true)
string(53) "phar://%s027.phar.php/b"
string(8) "/b/c.php"
bool(false)
string(8) "/b/d.php"
bool(false)
string(6) "/b.php"
bool(false)
string(6) "/e.php"
bool(false)
===DONE===
