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
$ret = exec(get_junction().' /? 2>&1', $out);
if (strpos($out[0], 'recognized')) {
	die('skip. junction.exe not found in PATH.');
}

?>
--FILE--
<?php
include_once __DIR__ . '/common.inc';
$old_dir = __DIR__;
$dirname = __DIR__ . "\\mnt\\test\\directory";
exec("mkdir " . $dirname, $output, $ret_val);
chdir(__DIR__ . "\\mnt\\test");
exec(get_junction()." junction directory", $output, $ret_val);
file_put_contents("junction\\a.php", "<?php echo \"I am included.\n\" ?>");
file_put_contents("junction\\b.php", "<?php echo \"I am included.\n\" ?>");
include "junction/a.php";
require_once "junction\\b.php";
print_r(scandir("junction"));
unlink("junction\\a.php");
unlink("junction\\b.php");
chdir($old_dir);
rmdir(__DIR__ . "\\mnt\\test\\directory");
rmdir(__DIR__ . "\\mnt\\test\\junction");
rmdir(__DIR__ . "\\mnt\\test");
rmdir(__DIR__ . "\\mnt");

?>
--EXPECT--
I am included.
I am included.
Array
(
    [0] => .
    [1] => ..
    [2] => a.php
    [3] => b.php
)
