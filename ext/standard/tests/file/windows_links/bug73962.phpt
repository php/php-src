--TEST--
Bug #73962 bug with symlink related to cyrillic directory
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' ) {
    die('skip windows only test');
}
include_once __DIR__ . '/common.inc';
skipIfSeCreateSymbolicLinkPrivilegeIsDisabled(__FILE__);
?>
--FILE--
<?php
include_once __DIR__ . '/common.inc';
$mountvol = get_mountvol();
$old_dir = __DIR__;
$dirname = '"' . __DIR__ . "\\mnt\\test\\новая папка" . '"';
exec("mkdir " . $dirname, $output, $ret_val);
chdir(__DIR__ . "\\mnt\\test");
$drive = substr(__DIR__, 0, 2);
$pathwithoutdrive = substr(__DIR__, 2);
$ret = exec($mountvol . " " . $drive . " /L", $output, $ret_val);
exec("mklink /d mounted_volume " . $ret, $output, $ret_val);
$fullpath = "mounted_volume" . $pathwithoutdrive;
exec("mklink /d mklink_symlink \"новая папка\"", $output, $ret_val);
file_put_contents("mklink_symlink\\a.php", "<?php echo \"I am included.\n\" ?>");
file_put_contents("$fullpath\\mnt\\test\\новая папка\\b.php", "<?php echo \"I am included.\n\" ?>");
var_dump(scandir("mklink_symlink"));
var_dump(scandir("$fullpath\\mnt\\test\\новая папка"));
var_dump(scandir("$fullpath\\mnt\\test\\mklink_symlink"));
var_dump(is_readable("$fullpath\\mnt\\test\\mklink_symlink\b.php"));
unlink("$fullpath\\mnt\\test\\новая папка\\b.php");
unlink("mklink_symlink\\a.php");
chdir($old_dir);
rmdir(__DIR__ . "\\mnt\\test\\новая папка");
rmdir(__DIR__ . "\\mnt\\test\\mklink_symlink");
rmdir(__DIR__ . "\\mnt\\test\\mounted_volume");
rmdir(__DIR__ . "\\mnt\\test");
rmdir(__DIR__ . "\\mnt");

?>
--EXPECT--
array(4) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(5) "a.php"
  [3]=>
  string(5) "b.php"
}
array(4) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(5) "a.php"
  [3]=>
  string(5) "b.php"
}
array(4) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
  [2]=>
  string(5) "a.php"
  [3]=>
  string(5) "b.php"
}
bool(true)
