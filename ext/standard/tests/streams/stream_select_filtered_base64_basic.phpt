--TEST--
stream_select() base64-decode filter basic usage
--FILE--
<?php
$file = fopen('php://temp', 'r+');

// Write complete base64 "Hello World"
fwrite($file, 'SGVsbG8gV29ybGQ=');

stream_filter_append($file, 'convert.base64-decode');
rewind($file);

$read = [$file];
$write = null;
$except = null;

$result = stream_select($read, $write, $except, 0);

if ($result !== false) {
    echo "stream_select succeeded\n";
    if (count($read) > 0) {
        echo fread($file, 1024) . "\n";
    }
} else {
    echo "stream_select failed\n";
}

fclose($file);
?>
--EXPECT--
stream_select succeeded
Hello World
