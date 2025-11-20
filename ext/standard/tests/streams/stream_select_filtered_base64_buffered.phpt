--TEST--
stream_select() base64-decode filter buffered data usage
--FILE--
<?php
$domain = (strtoupper(substr(PHP_OS, 0, 3) == 'WIN') ? STREAM_PF_INET : STREAM_PF_UNIX);
$pipes = stream_socket_pair($domain, STREAM_SOCK_STREAM, 0);
if ($pipes === false) {
    die("Failed to create socket pair");
}

list($read_pipe, $write_pipe) = $pipes;

stream_set_blocking($read_pipe, false);
stream_filter_append($read_pipe, 'convert.base64-decode');

// Write incomplete base64 data: "SGVs" decodes to "Hel" but "SGVs" is incomplete
fwrite($write_pipe, 'SGVs');
fflush($write_pipe);

$read = [$read_pipe];
$write = null;
$except = null;

$result = fread($read_pipe, 1024);
echo "Decoded content (before select): " . $result . "\n";

// Should succeed but stream should NOT be readable yet (data buffered in filter)
$result = stream_select($read, $write, $except, 0, 1000);
echo "After incomplete data - select result: " . $result . "\n";
echo "After incomplete data - readable streams: " . count($read) . "\n";

// Now complete the base64 sequence: "SGVsbG8=" decodes to "Hello"
fwrite($write_pipe, 'bG8=');
fflush($write_pipe);

$read = [$read_pipe];
$write = null;
$except = null;

// Now stream should be readable
$result = stream_select($read, $write, $except, 0, 1000);
echo "After complete data - select result: " . $result . "\n";
echo "After complete data - readable streams: " . count($read) . "\n";

if (count($read) > 0) {
    $content = fread($read_pipe, 1024);
    echo "Decoded content: " . $content . "\n";
}

fclose($read_pipe);
fclose($write_pipe);
?>
--EXPECT--
Decoded content (before select): Hel
After incomplete data - select result: 0
After incomplete data - readable streams: 0
After complete data - select result: 1
After complete data - readable streams: 1
Decoded content: lo
