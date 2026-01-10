--TEST--
GH-10469: Disallow open_basedir() with parent dir components (..)
--EXTENSIONS--
zend_test
--FILE--
<?php
ini_set('open_basedir', __DIR__);

$pathSeparator = PHP_OS_FAMILY !== 'Windows' ? ':' : ';';
$originalDir = __DIR__;
$tmpDir = $originalDir . '/gh10469_tmp';
@mkdir($tmpDir, 0777, true);
chdir($tmpDir);
ini_set('open_basedir', ini_get('open_basedir') . $pathSeparator . '.' . DIRECTORY_SEPARATOR . '..');
ini_set('open_basedir', ini_get('open_basedir') . $pathSeparator . '.' . DIRECTORY_SEPARATOR . '..' . DIRECTORY_SEPARATOR);
ini_set('open_basedir', ini_get('open_basedir') . $pathSeparator . '.' . DIRECTORY_SEPARATOR . 'a' . DIRECTORY_SEPARATOR);
ini_set('open_basedir', ini_get('open_basedir') . $pathSeparator . '.' . DIRECTORY_SEPARATOR . 'a');

chdir($originalDir);
var_dump(ini_get('open_basedir'));
var_dump(get_open_basedir());
?>
--CLEAN--
<?php
@rmdir(__DIR__ . '/gh10469_tmp');
?>
--EXPECTF--
string(%d) "%stests%c.%e..%c.%e..%e%c.%ea%e%c.%ea"
string(%d) "%stests%c%stests%c%stests%e%c%stests%egh10469_tmp%ea"
