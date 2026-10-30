--TEST--
Bug #72666 (stat cache clearing inconsistent - exec)
--FILE--
<?php
$filename = __DIR__ . '/bug72666_variation4.txt';

touch($filename, 1);
var_dump(filemtime($filename));
exec("touch $filename");
var_dump(filemtime($filename) > 1);


touch($filename, 1);
var_dump(filemtime($filename));
shell_exec("touch $filename");
var_dump(filemtime($filename) > 1);
?>
--CLEAN--
<?php
unlink(__DIR__ . '/bug72666_variation4.txt');
?>
--EXPECT--
int(1)
bool(true)
int(1)
bool(true)
