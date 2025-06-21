--TEST--
GH-13264: fgets() and stream_get_line() do not return false on filter fatal error
--SKIPIF--
<?php require 'filter_errors.inc'; filter_errors_skipif('zlib.inflate'); ?>
--FILE--
<?php
function create_stream()
{
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

    return $stream;
}

// Read the filtered stream line by line using fgets.
$stream = create_stream();
while (($line = fgets($stream)) !== false) {
    $error = error_get_last();
    if ($error !== null) {
        // An error is thrown but fgets didn't return false
        var_dump(error_get_last());
        var_dump($line);
    }
}
fclose($stream);
error_clear_last();

// Read the filtered stream line by line using stream_get_line.
$stream = create_stream();
while (($line = stream_get_line($stream, 0, "\n")) !== false) {
    $error = error_get_last();
    if ($error !== null) {
        // An error is thrown but stream_get_line didn't return false
        var_dump(error_get_last());
        var_dump($line);
    }
}
fclose($stream);
?>
--EXPECTF--

Notice: fgets(): zlib: data error in %s on line %d

Notice: stream_get_line(): zlib: data error in %s on line %d
