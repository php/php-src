--TEST--
Test file_put_contents() function : usage variation - various absolute and relative paths
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Only run on Windows");
?>
--FILE--
<?php
$mainDir = "filePutContentsVar7Win.dir";
$subDir = "filePutContentsVar7SubWin";
$absMainDir = __DIR__.DIRECTORY_SEPARATOR.$mainDir;
mkdir($absMainDir);
$absSubDir = $absMainDir.DIRECTORY_SEPARATOR.$subDir;
mkdir($absSubDir);

$old_dir_path = getcwd();
chdir(__DIR__);

$unixifiedDir = '/'. substr(str_replace(DIRECTORY_SEPARATOR, '/', $absSubDir), 3);

$filename = 'FileGetContentsVar7.tmp';
$absFile = $absSubDir. DIRECTORY_SEPARATOR .$filename;
$data = "This was the written data";

$res = file_put_contents($unixifiedDir.DIRECTORY_SEPARATOR.$filename, $data);
if ($res !== false) {
    $in = file_get_contents($absFile);
    if ($in === $data) {
       echo "Data written correctly\n";
    }
    else {
       echo "Data not written correctly or to correct place\n";
    }
    unlink($unixifiedDir.DIRECTORY_SEPARATOR.$filename);
} else {
   echo "No data written\n";
}

chdir($old_dir_path);
?>
--CLEAN--
<?php
$mainDir = "filePutContentsVar7Win.dir";
$subDir = "filePutContentsVar7SubWin";
$absMainDir = __DIR__.DIRECTORY_SEPARATOR.$mainDir;
$absSubDir = $absMainDir.DIRECTORY_SEPARATOR.$subDir;
$filename = 'FileGetContentsVar7.tmp';
$absFile = $absSubDir. DIRECTORY_SEPARATOR .$filename;
@unlink($absFile);
rmdir($absSubDir);
rmdir($absMainDir);
?>
--EXPECT--
Data written correctly
