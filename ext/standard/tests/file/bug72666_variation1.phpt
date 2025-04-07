--TEST--
Bug #72666 (stat cache clearing inconsistent - touch)
--FILE--
<?php
$filename = __DIR__ . '/bug72666_variation1.txt';

touch($filename);
var_dump(filemtime($filename) > 2);
touch($filename, 1);
var_dump(filemtime($filename));

?>
--CLEAN--
<?php
unlink(__DIR__ . '/bug72666_variation1.txt');
?>
--EXPECT--
bool(true)
int(1)
