--TEST--
Bug #74154 (Phar extractTo creates empty files)
--EXTENSIONS--
phar
zlib
--FILE--
<?php

$dir = __DIR__.'/bug74154';
mkdir($dir);
file_put_contents("$dir/1.txt", str_repeat('h', 64));
file_put_contents("$dir/2.txt", str_repeat('i', 64));
$phar = new PharData(__DIR__.'/bug74154.tar');
$phar->buildFromDirectory($dir);

$compPhar = $phar->compress(Phar::GZ);
unset($phar); //make sure that test.tar is closed
unlink(__DIR__.'/bug74154.tar');
unset($compPhar); //make sure that test.tar.gz is closed
$extractingPhar = new PharData(__DIR__.'/bug74154.tar.gz');
$extractingPhar->extractTo($dir.'_out');

var_dump(file_get_contents($dir.'_out/1.txt'));
var_dump(file_get_contents($dir.'_out/2.txt'));

?>
--CLEAN--
<?php
$dir = __DIR__.'/bug74154';
@unlink("$dir/1.txt");
@unlink("$dir/2.txt");
@rmdir($dir);
@unlink($dir.'_out/1.txt');
@unlink($dir.'_out/2.txt');
@rmdir($dir.'_out');
@unlink(__DIR__.'/bug74154.tar');
@unlink(__DIR__.'/bug74154.tar.gz');
?>
--EXPECT--
string(64) "hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh"
string(64) "iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"
