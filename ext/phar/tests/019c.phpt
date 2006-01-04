--TEST--
Phar: opendir test, recurse into
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
$file = "<?php
Phar::mapPhar('hio', false);
__HALT_COMPILER(); ?>";
$manifest = '';
$manifest .= pack('V', 1) . 'a' .     pack('VVVV', 1, time(),  0, 9);
$manifest .= pack('V', 3) . 'b/a' .   pack('VVVV', 1, time(),  9, 9);
$manifest .= pack('V', 5) . 'b/c/d' . pack('VVVV', 1, time(), 18, 9);
$manifest .= pack('V', 5) . 'bad/c' . pack('VVVV', 1, time(), 27, 9);
$file .= pack('VV', strlen($manifest) + 4, 4) .
	 $manifest .
	 pack('VV', crc32('a'), 1) . 'a' .
	 pack('VV', crc32('b'), 1) . 'b' .
	 pack('VV', crc32('c'), 1) . 'c' .
	 pack('VV', crc32('d'), 1) . 'd';
file_put_contents(dirname(__FILE__) . '/019c.phar.php', $file);
include dirname(__FILE__) . '/019c.phar.php';

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
		else
		{
			var_dump(file_get_contents($phar . $entry));
		}
	}
}

dump('phar://hio', '/');

?>
--CLEAN--
<?php function cleanup() { unlink(dirname(__FILE__) . '/019c.phar.php'); } ?>
--EXPECT--
string(11) "phar://hio/"
string(2) "/a"
bool(false)
string(1) "a"
string(2) "/b"
bool(true)
string(12) "phar://hio/b"
string(4) "/b/a"
bool(false)
string(1) "b"
string(4) "/b/c"
bool(true)
string(14) "phar://hio/b/c"
string(6) "/b/c/d"
bool(false)
string(1) "c"
string(4) "/bad"
bool(true)
string(14) "phar://hio/bad"
string(6) "/bad/c"
bool(false)
string(1) "d"
