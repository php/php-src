--TEST--
Memory corruption error if fp of just created file is closed before curl_close.
--CREDITS--
Alexey Shein <confik@gmail.com>
--EXTENSIONS--
curl
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init($host);

$temp_file = __DIR__ . '/curl_file_deleted_before_curl_close.tmp';
if (file_exists($temp_file)) {
    unlink($temp_file); // file should not exist before test
}

$handle = fopen($temp_file, 'w');

curl_setopt($ch, CURLOPT_STDERR, $handle);
curl_setopt($ch, CURLOPT_VERBOSE, 1);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

curl_exec($ch);

fclose($handle); // causes glibc memory error

//unlink($temp_file); // uncomment to test segfault (file not found on iowrite.c)

curl_close($ch);
echo "Closed correctly\n";
?>
--CLEAN--
<?php
unlink(__DIR__ . '/curl_file_deleted_before_curl_close.tmp');
?>
--EXPECT--
Closed correctly
