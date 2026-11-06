--TEST--
GH-25232 ftruncate() clears the stat cache
--CREDITS--
tolry (tobias.olry@gmail.com)
--FILE--
<?php
$file = tempnam(sys_get_temp_dir(), 'stat-demo-');

file_put_contents($file, 'abcdefgh');

$handle = fopen($file, 'r+');

var_dump(filesize($file));
var_dump(ftruncate($handle, 3));
var_dump(filesize($file));

fclose($handle);
unlink($file);
?>
--EXPECT--
int(8)
bool(true)
int(3)
