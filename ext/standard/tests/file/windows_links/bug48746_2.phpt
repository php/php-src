--TEST--
Bug#48746 - Junction not working properly

--CREDIT--
Venkat Raman Don (don.raman@microsoft.com)

--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' ) {
    die('skip windows only test');
}
$ret = exec('mklink bug48746_tmp.lnk ' . __FILE__ .' 2>&1', $out);
if (strpos($ret, 'privilege')) {
	die('skip. SeCreateSymbolicLinkPrivilege not enable for this user.');
}
unlink('mklink bug48746_tmp.lnk');
?>
--FILE--
<?php
$old_dir = __DIR__;
$dirname = __DIR__ . "\\mnt\\test\\directory";
exec("mkdir " . $dirname, $output, $ret_val);
chdir(__DIR__ . "\\mnt\\test");
$drive = substr(__DIR__, 0, 2);
$pathwithoutdrive = substr(__DIR__, 2);
$ret = exec("mountvol " . $drive . " /L", $output, $ret_val);
exec("mklink /j mounted_volume " . $ret, $output, $ret_val);
$fullpath = "mounted_volume" . $pathwithoutdrive;
exec("mklink /j mklink_junction directory", $output, $ret_val);
file_put_contents("mklink_junction\\a.php", "<?php echo \"I am included.\n\" ?>");
file_put_contents("$fullpath\\mnt\\test\\directory\\b.php", "<?php echo \"I am included.\n\" ?>");
print_r(scandir("mklink_junction"));
print_r(scandir("$fullpath\\mnt\\test\\directory"));
print_r(scandir("$fullpath\\mnt\\test\\mklink_junction"));
unlink("$fullpath\\mnt\\test\\directory\\b.php");
unlink("mklink_junction\\a.php");
chdir($old_dir);
rmdir(__DIR__ . "\\mnt\\test\\directory");
rmdir(__DIR__ . "\\mnt\\test\\mklink_junction");
rmdir(__DIR__ . "\\mnt\\test\\mounted_volume");
rmdir(__DIR__ . "\\mnt\\test");
rmdir(__DIR__ . "\\mnt");

?>
--EXPECT--
Array
(
    [0] => .
    [1] => ..
    [2] => a.php
    [3] => b.php
)
Array
(
    [0] => .
    [1] => ..
    [2] => a.php
    [3] => b.php
)
Array
(
    [0] => .
    [1] => ..
    [2] => a.php
    [3] => b.php
)
