--TEST--
Bug #77552 Uninitialized php_stream_statbuf in stat functions
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip windows only test');
}
?>
--FILE--
<?php
// Check lstat on a Windows junction to ensure that st_mode is zero
$tmpDir = __DIR__.'/test-bug77552';

$target = $tmpDir.'/folder/target';
mkdir($target, 0777, true);

$junction = $tmpDir.'/junction';
$cmd = sprintf('mklink /J "%s" "%s"', $junction, $target);
exec($cmd);

$stat = lstat($junction);
var_dump($stat['mode']);

?>
--CLEAN--
<?php
$tmpDir = __DIR__.'/test-bug77552';
$cmd = sprintf('rmdir /S /Q "%s"', $tmpDir);
exec($cmd);
?>
--EXPECT--
int(0)
