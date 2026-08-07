--TEST--
CURL post data larger than 2GB (to test CURLOPT_POSTFIELDSIZE_LARGE)
--INI--
memory_limit=3G
post_max_size=3G
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (PHP_INT_SIZE < 8) die('skip 64-bit only');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
if (!function_exists('pcntl_fork')) die("skip Process Control Functions not available");
?>
--EXTENSIONS--
curl
--FILE--
<?php

$socket = stream_socket_server("tcp://127.0.0.1:29999", $errno, $errstr);
if (!$socket) {
    die("socket $errno $errstr");
}

if (pcntl_fork()) {
    // we don't use server.inc because that has limits on post size

    $conn = stream_socket_accept($socket);

    do {
        $header = fgets($conn);
        if (preg_match('~Content-Length: (\d+)~', $header, $matches)) {
            $length = $matches[1];
        }
    } while (trim($header));
    
    $postdata = stream_get_contents($conn, $length);
    var_dump(strlen($postdata));

    fwrite($conn, "HTTP/1.1 204 No Content\r\n\r\n");
    fclose($conn);

    $status = 0;
    pcntl_wait($status);
} else {
    $size = 2 ** 31 + 100; // a little bit more than a signed 32-bit int */
    $data = str_repeat('a', $size);

    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "http://127.0.0.1:29999/");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_POST, 1);

    curl_setopt($ch, CURLOPT_POSTFIELDS, $data);

    $response = curl_exec($ch);

    $uploaded_size = curl_getinfo($ch, CURLINFO_SIZE_UPLOAD_T);
    var_dump($uploaded_size);
}

fclose($socket);
?>
--EXPECT--
int(2147483748)
int(2147483748)
