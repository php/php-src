--TEST--
Bug #78220 (Can't access OneDrive folder)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die("skip this test is for Windows platforms only");
?>
--FILE--
<?php
$onedrive_dirs = array_unique([getenv('OneDrive'), getenv('OneDriveCommercial')]);
foreach ($onedrive_dirs as $dir) {
    if ($dir && scandir($dir) === FALSE) {
        echo "can't scan $dir\n";
    }
}
?>
--EXPECT--
