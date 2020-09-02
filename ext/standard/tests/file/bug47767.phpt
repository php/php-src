--TEST--
bug #47767 (include_once does not resolve windows symlinks or junctions)
--CREDITS--
Venkat Raman Don
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' ) {
		die('skip windows only test');
}
include_once __DIR__ . '/windows_links/common.inc';
skipIfSeCreateSymbolicLinkPrivilegeIsDisabled(__FILE__);
?>
--FILE--
<?php
echo "Testing include_once using file symbolic link\n";
$filename = __DIR__ . '\\a.php';
$content = '<?php echo "I am included\n" ?>';
file_put_contents($filename, $content);
$softlinkname = __DIR__ . '\\a_slink.php';
symlink($filename, $softlinkname);
include_once("$filename");
include_once("$softlinkname");
include_once("$softlinkname");

echo "Testing include_once using directory symbolic link\n";
$softdirlinkname = __DIR__ . "\\a_dir";
symlink(__DIR__, $softdirlinkname);
include_once("$softdirlinkname" . '\\a.php');

echo "Testing include_once using junction points\n";
$junctionname = __DIR__ . '\\a_jdir';
exec("mklink /J $junctionname " . __DIR__);
include_once("$junctionname" . '\\a.php');

unlink($filename);
unlink($softlinkname);
rmdir($softdirlinkname);
rmdir($junctionname);
?>
--EXPECT--
Testing include_once using file symbolic link
I am included
Testing include_once using directory symbolic link
Testing include_once using junction points
