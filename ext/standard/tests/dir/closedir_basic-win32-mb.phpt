--TEST--
Test closedir() function : basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/*
 * Test basic functionality of closedir()
 */

echo "*** Testing closedir() : basic functionality ***\n";

$base_dir = __DIR__;
$dir_path = $base_dir . '/私はガラスを食べられますclosedir_basic';
mkdir($dir_path);

echo "\n-- Call closedir() with no arguments: --\n";
$dh1 = opendir($dir_path);
var_dump(closedir());
echo "-- Check Directory Handle: --\n";
var_dump($dh1);

echo "\n-- Call closedir() with \$dir_handle argument supplied: --\n";
$dh2 = opendir($dir_path);

if ((int)$dh1 === (int)$dh2) {
    echo "\nNo new resource created\n";
}
var_dump(closedir($dh2));
echo "-- Check Directory Handle: --\n";
var_dump($dh2);
?>
--CLEAN--
<?php
$base_dir = __DIR__;
$dir_path = $base_dir . '/私はガラスを食べられますclosedir_basic';
rmdir($dir_path);
?>
--EXPECTF--
*** Testing closedir() : basic functionality ***

-- Call closedir() with no arguments: --
NULL
-- Check Directory Handle: --
resource(%d) of type (Unknown)

-- Call closedir() with $dir_handle argument supplied: --
NULL
-- Check Directory Handle: --
resource(%d) of type (Unknown)
