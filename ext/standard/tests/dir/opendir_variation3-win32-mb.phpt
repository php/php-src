--TEST--
Test opendir() function : usage variations - open a directory twice
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
  die("skip Valid only on Windows");
}
?>
--FILE--
<?php
/*
 * Call opendir() twice with the same directory as $path argument
 */

echo "*** Testing opendir() : usage variation ***\n";

$path = __DIR__ . "/私はガラスを食べられますopendir_variation3";
mkdir($path);

echo "\n-- Open directory first time: --\n";
var_dump($dh1 = opendir($path));

echo "\n-- Open directory second time: --\n";
var_dump($dh2 = opendir($path));

if ($dh1 !== $dh2) {
    echo "\nNew resource created\n";
} else {
    echo "\nNo new resource created\n";
}

closedir($dh1);
closedir($dh2);
?>
--CLEAN--
<?php
$path = __DIR__ . "/私はガラスを食べられますopendir_variation3";
rmdir($path);
?>
--EXPECTF--
*** Testing opendir() : usage variation ***

-- Open directory first time: --
resource(%d) of type (stream)

-- Open directory second time: --
resource(%d) of type (stream)

New resource created
