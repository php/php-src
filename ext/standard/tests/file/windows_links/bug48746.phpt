--TEST--
Bug#48746 - Junction not working properly

--CREDITS--
Venkat Raman Don (don.raman@microsoft.com)

--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' ) {
    die('skip windows only test');
}
include_once __DIR__ . '/common.inc';
$cmd = "mklink /?";
$ret = @exec($cmd, $output, $return_val);
if (count($output) == 0) {
    die("mklink.exe not found in PATH");
}
?>
--FILE--
<?php
include_once __DIR__ . '/common.inc';
$mountvol = get_mountvol();
$old_dir = __DIR__;
$dirname = __DIR__ . "\\mnt\\test\\directory";
mkdir($dirname, 0700, true);
chdir(__DIR__ . "\\mnt\\test");
$drive = substr(__DIR__, 0, 2);
$pathwithoutdrive = substr(__DIR__, 2);
$ret = exec($mountvol . " " . $drive . " /L", $output, $ret_val);
exec("mklink /j mounted_volume " . $ret, $output, $ret_val);
$fullpath = "mounted_volume" . $pathwithoutdrive;
exec("mklink /j mklink_junction directory", $output, $ret_val);
var_dump(file_exists("directory"));
var_dump(file_exists("mklink_junction"));
var_dump(file_exists("mounted_volume"));
var_dump(file_exists("$fullpath"));
var_dump(is_dir("mklink_junction"));
var_dump(is_dir("$fullpath"));
var_dump(is_readable("mklink_junction"));
var_dump(is_writeable("$fullpath"));
chdir($old_dir);

rmdir(__DIR__ . "\\mnt\\test\\directory");
rmdir(__DIR__ . "\\mnt\\test\\mklink_junction");
rmdir(__DIR__ . "\\mnt\\test\\mounted_volume");
rmdir(__DIR__ . "\\mnt\\test");
rmdir(__DIR__ . "\\mnt");

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
