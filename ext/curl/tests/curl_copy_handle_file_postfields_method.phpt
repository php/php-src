--TEST--
curl_copy_handle() does not restore file post fields after the request method changes
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

foreach ([
    'CURLOPT_HTTPGET' => [CURLOPT_HTTPGET, true],
    'CURLOPT_POST' => [CURLOPT_POST, false],
    'CURLOPT_UPLOAD' => [CURLOPT_UPLOAD, false],
    'CURLOPT_NOBODY' => [CURLOPT_NOBODY, true],
] as $name => [$option, $value]) {
    $handle = curl_init("{$host}/get.inc?test=method");
    curl_setopt($handle, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($handle, CURLOPT_POSTFIELDS, ['file' => new CURLStringFile('body', 'file.txt')]);
    curl_setopt($handle, $option, $value);

    $copy = curl_copy_handle($handle);
    var_dump($name, curl_exec($handle), curl_exec($copy));
}
?>
--EXPECT--
string(15) "CURLOPT_HTTPGET"
string(3) "GET"
string(3) "GET"
string(12) "CURLOPT_POST"
string(3) "GET"
string(3) "GET"
string(14) "CURLOPT_UPLOAD"
string(3) "GET"
string(3) "GET"
string(14) "CURLOPT_NOBODY"
string(0) ""
string(0) ""
