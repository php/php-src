--TEST--
Bug GH-9674 (RecursiveDirectoryIterator regression wrt. junctions)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") die("skip test is for Windows only");
?>
--FILE--
<?php
$dir = __DIR__ . "/gh9674";
mkdir($dir);
touch("$dir/a.txt");
touch("$dir/b.txt");
mkdir("{$dir}_ext");
touch("{$dir}_ext/c.txt");
exec("mklink /j " . str_replace("/", "\\", "$dir/ext") . " " . realpath("{$dir}_ext"));

$directory = new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::FOLLOW_SYMLINKS);
$iterator = new RecursiveIteratorIterator($directory);
foreach ($iterator as $it) {
    var_dump($it->getFilename());
}
?>
--CLEAN--
<?php
$dir = __DIR__ . "/gh9674";
@unlink("$dir/a.txt");
@unlink("$dir/b.txt");
@rmdir("$dir/ext");
@unlink("{$dir}_ext/c.txt");
@rmdir("{$dir}_ext");
@rmdir($dir);
?>
--EXPECT--
string(1) "."
string(2) ".."
string(5) "a.txt"
string(5) "b.txt"
string(1) "."
string(2) ".."
string(5) "c.txt"
