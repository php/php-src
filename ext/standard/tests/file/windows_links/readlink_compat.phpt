--TEST--
Test readlink bc behaviour on Windows
--DESCRIPTION--
Checks readlink is backward-compatible with PHP-7.3 and below
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip windows only test');
}
exec('fltmc', $output, $exitCode);
if ($exitCode !== 0) {
    die('skip administrator privileges required');
}
?>
--FILE--
<?php
$tmpDir = __DIR__ . '\\mnt';
mkdir($tmpDir);

// mounted volume
$volume = trim(exec('mountvol C: /L'));
exec(sprintf('mountvol "%s" %s', $tmpDir, $volume));
var_dump(readlink($tmpDir));
exec(sprintf('mountvol "%s" /D', $tmpDir));

mkdir($tmpDir . '\\test\\directory', 0777, true);
chdir($tmpDir . '\\test');

// junction to a volume (same as a mounted volume)
$link = $tmpDir . '\\test\\volume_junction';
exec(sprintf('mklink /J "%s" %s', $link, $volume));
var_dump(readlink($link));
rmdir($link);

// junction to a directory
$link = $tmpDir . '\\test\\directory_junction';
$target = $tmpDir . '\\test\\directory';
exec(sprintf('mklink /J "%s" "%s"', $link, $target));
var_dump(readlink($link));
rmdir($link);

// symlink to a directory (absolute and relative)
$link = $tmpDir . '\\test\\directory_symlink';
$target = $tmpDir . '\\test\\directory';
exec(sprintf('mklink /D "%s" "%s"', $link, $target));
var_dump(readlink($link));
rmdir($link);
exec(sprintf('mklink /D "%s" directory', $link));
var_dump(readlink($link));
rmdir($link);

// create a file to link to
$filename = $tmpDir . '\\test\\directory\\a.php';
$fh = fopen($filename, 'w');
fclose($fh);

// symlink to a file (absolute and relative)
$link = $tmpDir . '\\test\\file_symlink';
exec(sprintf('mklink "%s" "%s"', $link, $filename));
var_dump(readlink($link));
unlink($link);
exec(sprintf('mklink "%s" directory\\a.php', $link));
var_dump(readlink($link));
unlink($link);

// unexpected behaviour
echo "\n*** Unexpected behaviour when not a reparse point\n";
var_dump(readlink($tmpDir . '\\test\\directory'));
var_dump(readlink($filename));

unlink($filename);

chdir(__DIR__);
rmdir($tmpDir . '\\test\\directory');
rmdir($tmpDir . '\\test');
rmdir($tmpDir);
?>
--EXPECTF--
string(3) "C:\"
string(3) "C:\"
string(%d) "%s\mnt\test\directory"
string(%d) "%s\mnt\test\directory"
string(%d) "%s\mnt\test\directory"
string(%d) "%s\mnt\test\directory\a.php"
string(%d) "%s\mnt\test\directory\a.php"

*** Unexpected behaviour when not a reparse point
string(%d) "%s\mnt\test\directory"
string(%d) "%s\mnt\test\directory\a.php"
