--TEST--
Phar: opendir test, recurse into
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (version_compare(PHP_VERSION, "6.0", "<")) die("skip Unicode support required");
?>
--INI--
phar.require_hash=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.php';
$pname = 'phar://' . $fname;
$file = b"<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>";

$files = array();
$files['a'] = 'a';
$files['b/a'] = 'b';
$files['b/c/d'] = 'c';
$files['bad/c'] = 'd';

include 'files/phar_test.inc';
include $fname;

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

dump('phar://hio', '/');

?>
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.php'); ?>
--EXPECT--
unicode(11) "phar://hio/"
unicode(2) "/a"
bool(false)
unicode(2) "/b"
bool(true)
unicode(12) "phar://hio/b"
unicode(4) "/b/a"
bool(false)
unicode(4) "/b/c"
bool(true)
unicode(14) "phar://hio/b/c"
unicode(6) "/b/c/d"
bool(false)
unicode(4) "/bad"
bool(true)
unicode(14) "phar://hio/bad"
unicode(6) "/bad/c"
bool(false)
