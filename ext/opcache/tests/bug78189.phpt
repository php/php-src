--TEST--
Bug #78189 (file cache strips last character of uname hash)
--SKIPIF--
<?php
if (!extension_loaded('Zend OPcache')) die('skip opcache extension not available');
if (substr(PHP_OS, 0, 3) !== 'WIN') die('skip this test is for Windows platforms only');
?>
--INI--
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_cache={TMP}
opcache.file_cache_only=1
--FILE--
<?php
$tmpdir = sys_get_temp_dir();
$pattern = $tmpdir . '/*/*/' . str_replace(':', '', __DIR__) . '/bug78189.php.bin';
foreach (glob($pattern) as $filename) {
    var_dump(preg_match('~/[0-9a-f]{32}/~', substr($filename, strlen($tmpdir), 34)));
}
?>
--EXPECT--
int(1)
