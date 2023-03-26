--TEST--
GH-10469: Disallow open_basedir() with parent dir components (..)
--FILE--
<?php
ini_set('open_basedir', __DIR__);

$originalDir = __DIR__;
$tmpDir = $originalDir . '/gh10469_tmp';
@mkdir($tmpDir, 0777, true);
chdir($tmpDir);
ini_set('open_basedir', ini_get('open_basedir') . ':./..');
ini_set('open_basedir', ini_get('open_basedir') . ':./../');
ini_set('open_basedir', ini_get('open_basedir') . ':/a/');

chdir($originalDir);
var_dump(ini_get('open_basedir'));
?>
--CLEAN--
<?php
@rmdir(__DIR__ . '/gh10469_tmp');
?>
--EXPECTF--
string(%d) "%stests"
