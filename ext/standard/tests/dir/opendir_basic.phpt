--TEST--
Test opendir() function : basic functionality 
--FILE--
<?php
/* Prototype  : mixed opendir(string $path[, resource $context])
 * Description: Open a directory and return a dir_handle 
 * Source code: ext/standard/dir.c
 */

/*
 * Test basic functionality of opendir() with absolute and relative paths as $path argument
 */

echo "*** Testing opendir() : basic functionality ***\n";

$base_dir_path = dirname(__FILE__);

$level_one_dir_name = "level_one";
$level_one_dir_path = "$base_dir_path/$level_one_dir_name";

$level_two_dir_name = "level_two";
$level_two_dir_path = "$base_dir_path/$level_one_dir_name/$level_two_dir_name";

// create temporary directories - will remove in CLEAN section
mkdir($level_one_dir_path);
mkdir($level_two_dir_path);

echo "\n-- Testing opendir() with absolute path: --\n";
var_dump($dh1 = opendir($level_one_dir_path));


echo "\n-- Testing opendir() with relative paths: --\n";
var_dump(chdir($level_one_dir_path));
var_dump($dh2 = opendir($level_two_dir_name));

echo "\n-- Close directory handles: --\n";
closedir($dh1);
var_dump($dh1);
closedir($dh2);
var_dump($dh2);
?>
===DONE===
--CLEAN--
<?php
$file_path = dirname(__FILE__);
rmdir("$file_path/level_one/level_two");
rmdir("$file_path/level_one");
?>
--EXPECTF--
*** Testing opendir() : basic functionality ***

-- Testing opendir() with absolute path: --
resource(%d) of type (stream)

-- Testing opendir() with relative paths: --
bool(true)
resource(%d) of type (stream)

-- Close directory handles: --
resource(%d) of type (Unknown)
resource(%d) of type (Unknown)
===DONE===
