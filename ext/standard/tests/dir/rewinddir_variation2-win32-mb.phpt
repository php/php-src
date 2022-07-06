--TEST--
Test rewinddir() function : usage variations - operate on a closed directory
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/*
 * Open and close a directory handle then call rewinddir() to test behaviour
 */

echo "*** Testing rewinddir() : usage variations ***\n";

$dir_path = __DIR__ . '/私はガラスを食べられますrewinddir_variation2';
mkdir($dir_path);

echo "\n-- Create the directory handle, read and close the directory --\n";
var_dump($dir_handle = opendir($dir_path));
var_dump(readdir($dir_handle));
closedir($dir_handle);

echo "\n-- Call to rewinddir() --\n";
try {
    var_dump(rewinddir($dir_handle));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
$dir_path = __DIR__ . '/私はガラスを食べられますrewinddir_variation2';
rmdir($dir_path);
?>
--EXPECTF--
*** Testing rewinddir() : usage variations ***

-- Create the directory handle, read and close the directory --
resource(%d) of type (stream)
string(%d) "%s"

-- Call to rewinddir() --
rewinddir(): %s is not a valid Directory resource
