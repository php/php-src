--TEST--
Bug #76859 (stream_get_line skips data if used with filters)
--FILE--
<?php

$data = '123';

$fh = fopen('php://memory', 'r+b');
fwrite($fh, $data);
rewind($fh);
stream_filter_append($fh, 'string.rot13', STREAM_FILTER_READ);

$out = '';
while (!feof($fh)) {
    $out .= stream_get_line($fh, 1024);
}

fclose($fh);

echo strlen($out) . "\n";
?>
--EXPECT--
3
