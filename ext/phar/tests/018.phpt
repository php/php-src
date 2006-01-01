--TEST--
opendir test, root directory
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php
function cleanup() { unlink(dirname(__FILE__) . '/008_phar.php'); }
register_shutdown_function('cleanup');
$file = "<?php
Phar::mapPhar('hio', true);
__HALT_COMPILER(); ?>";
$manifest = '';
$manifest .= pack('V', 1) . 'a' . pack('VVVV', 1, time(), 0, 9);
$manifest .= pack('V', 3) . 'b/a' . pack('VVVV', 1, time(), 0, 9);
$file .= pack('VV', strlen($manifest) + 4, 2) .
	 $manifest .
	 pack('VV', crc32('a'), 1) . 'a' .
	 pack('VV', crc32('b'), 1) . 'b';
file_put_contents(dirname(__FILE__) . '/008_phar.php', $file);
include dirname(__FILE__) . '/008_phar.php';
$dir = opendir('phar://hio/');
while (false !== ($a = readdir($dir))) {
	var_dump($a);
	var_dump(is_dir('phar://hio/' . $a));
}
?>
--EXPECT--
string(1) "a"
bool(false)
string(1) "b"
bool(true)