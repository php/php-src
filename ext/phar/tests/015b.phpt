--TEST--
Phar::mapPhar valid file (bzip2)
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";
if (!extension_loaded("bz2")) print "skip bz2 not present"; ?>
--FILE--
<?php
$file = "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

$files = array();
$files['a'] = array('Hello World', pack('H*', '425a6839314159265359d872012f00000157800010400000400080060490002000220686d420c988c769e8281f8bb9229c28486c39009780'));
$manifest = '';
foreach($files as $name => $cont) {
	$manifest .= pack('V', strlen($name)) . $name . pack('VVVVC', strlen($cont[0]), time(), strlen($cont[1]), crc32($cont[0]), 0x02);
}
$alias = 'hio';
$manifest = pack('VnV', count($files), 0x0800, strlen($alias)) . $alias . $manifest;
$file .= pack('V', strlen($manifest)) . $manifest;
foreach($files as $cont)
{
	$file .= $cont[1];
}

file_put_contents(dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php', $file);
include dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
var_dump(file_get_contents('phar://hio/a'));
?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
string(11) "Hello World"
