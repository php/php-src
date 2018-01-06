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
exec("mkdir " . $dirname, $output, $ret_val);
chdir(__DIR__ . "\\mnt\\test");
$drive = substr(__DIR__, 0, 2);
$pathwithoutdrive = substr(__DIR__, 2);
$ret = exec($mountvol . " " . $drive . " /L", $output, $ret_val);
exec("mklink /j mounted_volume " . $ret, $output, $ret_val);
$fullpath = "mounted_volume" . $pathwithoutdrive;
exec("mklink /j mklink_junction directory", $output, $ret_val);
file_put_contents("mklink_junction\\a.php", "<?php echo \"I am included.\n\" ?>");
include_once "mklink_junction\\a.php";
file_put_contents("$fullpath\\mnt\\test\\directory\\b.php", "<?php echo \"I am included.\n\" ?>");
require "$fullpath\\mnt\\test\\directory\\b.php";
file_put_contents("$fullpath\\mnt\\test\\mklink_junction\\c.php", "<?php echo \"I am included.\n\" ?>");
require_once "$fullpath\\mnt\\test\\mklink_junction\\c.php";
var_dump(is_file("mklink_junction\\a.php"));
var_dump(is_file("$fullpath\\mnt\\test\\directory\\b.php"));
var_dump(is_file("$fullpath\\mnt\\test\\mklink_junction\\c.php"));
unlink("$fullpath\\mnt\\test\\directory\\b.php");
unlink("$fullpath\\mnt\\test\\mklink_junction\\c.php");
unlink("mklink_junction\\a.php");
chdir($old_dir);
rmdir(__DIR__ . "\\mnt\\test\\directory");
rmdir(__DIR__ . "\\mnt\\test\\mklink_junction");
rmdir(__DIR__ . "\\mnt\\test\\mounted_volume");
rmdir(__DIR__ . "\\mnt\\test");
rmdir(__DIR__ . "\\mnt");

?>
--EXPECT--
I am included.
I am included.
I am included.
bool(true)
bool(true)
bool(true)
