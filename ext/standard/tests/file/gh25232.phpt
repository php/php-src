--TEST--
GH-25232: ftruncate() does not invalidate stat cache
--FILE--
<?php
$file = __DIR__ . '/gh25232.tmp';
file_put_contents($file, 'abcdefgh');
$handle = fopen($file, 'r+');

var_dump(filesize($file));
var_dump(ftruncate($handle, 3));
var_dump(filesize($file));

fclose($handle);

$obj = new SplFileObject($file, 'r+');
var_dump(filesize($file));
var_dump($obj->ftruncate(1));
var_dump(filesize($file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25232.tmp');
?>
--EXPECT--
int(8)
bool(true)
int(3)
int(3)
bool(true)
int(1)
