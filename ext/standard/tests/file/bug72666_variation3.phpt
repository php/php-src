--TEST--
Bug #72666 (stat cache clearing inconsistent - plain wrapper)
--FILE--
<?php
$filename = __DIR__ . '/bug72666_variation3.txt';

file_put_contents($filename, "test");
$mtime1 = filemtime($filename);
$fd = fopen($filename, "w");
sleep(1);
var_dump(fwrite($fd, "data"));
$mtime2 = filemtime($filename);
var_dump($mtime2 > $mtime1);
?>
--CLEAN--
<?php
unlink(__DIR__ . '/bug72666_variation3.txt');
?>
--EXPECT--
int(4)
bool(true)
