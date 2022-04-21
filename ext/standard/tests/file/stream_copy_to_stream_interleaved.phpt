--TEST--
stream_copy_to_stream() tests with interleaved read/write calls
--FILE--
<?php
define('WIN', substr(PHP_OS, 0, 3) == 'WIN');

$initial_file = __DIR__.'/bug38086.txt';
$new_file = __DIR__.'/stream_copy_to_stream_interleaved.txt';

$src = fopen($initial_file, 'r');

$dest = fopen($new_file, 'w');

var_dump(fread($src, 10));
var_dump(fwrite($dest, "foo"));
var_dump(stream_copy_to_stream($src, $dest, 10));
var_dump(ftell($src));
var_dump(ftell($dest));
var_dump(fread($src, 10));
var_dump(fwrite($dest, "bar"));
var_dump(stream_copy_to_stream($src, $dest, 10));
var_dump(ftell($src));
var_dump(ftell($dest));
fclose($src); fclose($dest);

if (WIN) {
  var_dump(str_replace("\r\n","\n", file_get_contents($new_file)));
} else {
  var_dump(file_get_contents($new_file));
}
unlink($new_file);

echo "Done\n";
?>
--EXPECTF--
string(10) "Another da"
int(3)
int(10)
int(20)
int(13)
string(10) " pains of "
int(3)
int(10)
int(40)
int(26)
string(26) "fooy
When thebarlife won't"
Done
