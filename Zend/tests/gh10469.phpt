--TEST--
GH-10469: Disallow escaping cwd in open_basedir()
--FILE--
<?php
$rootDir = __DIR__;
ini_set('open_basedir', $rootDir);

$tmpDir = $rootDir . '/gh10469_tmp/a/a';
@mkdir($rootDir . '/gh10469_tmp/a/a', 0777, true);

chdir($rootDir . '/gh10469_tmp/a/a');
ini_set('open_basedir', ini_get('open_basedir') . ':./..');
ini_set('open_basedir', ini_get('open_basedir') . ':./../');
ini_set('open_basedir', ini_get('open_basedir') . ':./..:');

chdir($rootDir . '/gh10469_tmp/a');
ini_set('open_basedir', ini_get('open_basedir') . ':./a/../../');
ini_set('open_basedir', ini_get('open_basedir') . ':./a/../a/.././../');

ini_set('open_basedir', ini_get('open_basedir') . ':./a/../');
ini_set('open_basedir', ini_get('open_basedir') . ':./a/../a/');

chdir($rootDir);
var_dump(ini_get('open_basedir'));
?>
--CLEAN--
<?php
@rmdir(__DIR__ . '/gh10469_tmp/a/b');
@rmdir(__DIR__ . '/gh10469_tmp/a');
@rmdir(__DIR__ . '/gh10469_tmp');
?>
--EXPECTF--
string(%d) "%stests:./a/../:./a/../a/"
