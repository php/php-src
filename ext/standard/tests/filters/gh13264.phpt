--TEST--
GH-81475: Memory leak during stream filter failure
--SKIPIF--
<?php require 'filter_errors.inc'; filter_errors_skipif('zlib.inflate'); ?>
--FILE--
<?php
// Prepare a big enough input so that it is not entirely buffered
$stream = fopen('php://memory', 'r+');
$content = '';
for ($i = 0; $i < 10000; $i++) {
	$content .= "Hello $i\n";
}
fwrite($stream, gzcompress($content));

// Mess up the checksum
fseek($stream, -1, SEEK_CUR);
fwrite($stream, '1');

// Rewind and add the zlib filter
rewind($stream);
stream_filter_append($stream, 'zlib.inflate', STREAM_FILTER_READ, ['window' => 15]);

// Read the filtered stream line by line.
while (($line = fgets($stream)) !== false) {
	$error = error_get_last();
	if ($error !== null) {
		// An error is thrown but fgets didn't return false
		var_dump(error_get_last());
		var_dump($line);
	}
}

fclose($stream);
?>
--EXPECTF--

Notice: fgets(): zlib: data error in %s on line %d
array(4) {
  ["type"]=>
  int(8)
  ["message"]=>
  string(25) "fgets(): zlib: data error"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
}
string(7) "Hello 6"

