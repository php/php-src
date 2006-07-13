--TEST--
Bug #38086 (stream_copy_to_stream() returns 0 when maxlen is bigger than the actual length)
--FILE--
<?php

$initial_file = dirname(__FILE__).'/bug38086.txt';
$new_file = dirname(__FILE__).'/bug38086_1.txt';

$src = fopen($initial_file, 'r');
stream_filter_append($src, "string.rot13", STREAM_FILTER_READ);

$dest = fopen($new_file, 'w');
var_dump(stream_copy_to_stream($src, $dest));
fclose($src); fclose($dest);

var_dump(file_get_contents($new_file));
unlink($new_file);

$src = fopen($initial_file, 'r');
stream_filter_append($src, "string.rot13", STREAM_FILTER_READ);

$dest = fopen($new_file, 'w');
var_dump(stream_copy_to_stream($src, $dest, 10000));
fclose($src); fclose($dest);

var_dump(file_get_contents($new_file));
unlink($new_file);

echo "Done\n";
?>
--EXPECTF--	
int(134)
string(134) "Nabgure qnl
Jura gur cnvaf bs yvsr jba'g one zl jnl
V'yy oernx gurfr punvaf
Gung ubyq zr qbja
V'yy grne lbh qbja vagb zl cevingr uryy
"
int(134)
string(134) "Nabgure qnl
Jura gur cnvaf bs yvsr jba'g one zl jnl
V'yy oernx gurfr punvaf
Gung ubyq zr qbja
V'yy grne lbh qbja vagb zl cevingr uryy
"
Done
