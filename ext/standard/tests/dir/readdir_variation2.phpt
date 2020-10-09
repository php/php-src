--TEST--
Test readdir() function : usage variations - empty directories
--FILE--
<?php
/*
 * Pass readdir() a directory handle pointing to an empty directory to test behaviour
 */

echo "*** Testing readdir() : usage variations ***\n";

$path = __DIR__ . '/readdir_variation2';
mkdir($path);
$dir_handle = opendir($path);

echo "\n-- Pass an empty directory to readdir() --\n";
function mysort($a,$b) {
    return strlen($a) > strlen($b) ? 1 : -1;
}
$entries = array();
while(FALSE !== ($file = readdir($dir_handle))){
    $entries[] = $file;
}

closedir($dir_handle);

usort($entries, "mysort");
foreach($entries as $entry) {
    var_dump($entry);
}
?>
--CLEAN--
<?php
$path = __DIR__ . '/readdir_variation2';
rmdir($path);
?>
--EXPECT--
*** Testing readdir() : usage variations ***

-- Pass an empty directory to readdir() --
string(1) "."
string(2) ".."
