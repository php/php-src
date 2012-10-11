--TEST--
Bug #63240 stream_get_line return contains delimiter string
--FILE--
<?php

$file = 'php://temp';
$fh   = fopen($file, 'r+');
$data = str_repeat( '.', 8189 ) . 'MMMM';

fwrite($fh, $data);
fseek($fh, 0);

$delimiter = "MM";

stream_get_line($fh, 4096, $delimiter);
stream_get_line($fh, 4096, $delimiter);
if ($delimiter === stream_get_line($fh, 4096, $delimiter)) {
  echo "BROKEN\n";
} else {
  echo "OK\n";
}
fclose($fh);

--EXPECT--
OK
