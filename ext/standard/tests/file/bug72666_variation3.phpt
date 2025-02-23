--TEST--
Bug #72666 (stat cache clearing inconsistent - plain wrapper)
--FILE--
<?php
$filename = __DIR__ . '/bug72666_variation3.txt';

file_put_contents($filename, "test");
$fd = fopen($filename, "r");
$atime1 = fileatime($filename);
sleep(1);
var_dump(fread($fd, 4));
$atime2 = fileatime($filename);
$mtime1 = filemtime($filename);
fclose($fd);
$fd = fopen($filename, "w");
sleep(1);
var_dump(fwrite($fd, "data"));
$mtime2 = filemtime($filename);
if (substr(PHP_OS, 0, 3) == 'WIN') {
    // Windows do not hundle atime
    var_dump($atime2 == $atime1);
} else {
    var_dump($atime2 > $atime1);
}
var_dump($mtime2 > $mtime1);
?>
--CLEAN--
<?php
unlink(__DIR__ . '/bug72666_variation3.txt');
?>
--EXPECT--
string(4) "test"
int(4)
bool(true)
bool(true)
