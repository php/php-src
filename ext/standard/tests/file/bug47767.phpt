--TEST--
bug #47767 (include_once does not resolve windows symlinks or junctions)
--CREDITS--
Venkat Raman Don
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' ) {
		die('skip windows only test');
}
if(PHP_WINDOWS_VERSION_MAJOR < 6)  {
        die('skip windows version 6.0+ only test');
}

$ret = exec('mklink rename_variation13tmp.lnk ' . __FILE__ .' 2>&1', $out);
if (strpos($ret, 'privilege')) {
	die('skip. SeCreateSymbolicLinkPrivilege not enable for this user.');
}
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