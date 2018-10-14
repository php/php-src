--TEST--
Test opendir() function : usage variations - open a directory twice
--FILE--
<?php
/* Prototype  : mixed opendir(string $path[, resource $context])
 * Description: Open a directory and return a dir_handle
 * Source code: ext/standard/dir.c
 */

/*
 * Call opendir() twice with the same directory as $path argument
 */

echo "*** Testing opendir() : usage variation ***\n";

$path = dirname(__FILE__) . "/opendir_variation3";
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
===DONE===
--CLEAN--
<?php
$path = dirname(__FILE__) . "/opendir_variation3";
rmdir($path);
?>
--EXPECTF--
*** Testing opendir() : usage variation ***

-- Open directory first time: --
resource(%d) of type (stream)

-- Open directory second time: --
resource(%d) of type (stream)

New resource created
===DONE===
