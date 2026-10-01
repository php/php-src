--TEST--
Test curl_copy_handle() with CURLOPT_SEEKFUNCTION
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$body = 'Hello cURL seek!';
$offset = 0;
$seekCalls = 0;

$ch = curl_init("{$host}/get.inc?test=redirect");
curl_setopt($ch, CURLOPT_UPLOAD, true);
curl_setopt($ch, CURLOPT_INFILESIZE, strlen($body));
curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_READFUNCTION, function ($ch, $fd, int $length) use ($body, &$offset) {
    $chunk = substr($body, $offset, $length);
    $offset += strlen($chunk);
    return $chunk;
});
curl_setopt($ch, CURLOPT_SEEKFUNCTION, function ($ch, int $position, int $origin) use (&$offset, &$seekCalls) {
    if ($origin !== SEEK_SET) {
        return CURL_SEEKFUNC_CANTSEEK;
    }
    $seekCalls++;
    $offset = $position;
    return CURL_SEEKFUNC_OK;
});

// The copied handle must inherit the seek callback; exercise it on the copy
// after freeing the original.
$ch2 = curl_copy_handle($ch);
unset($ch);

$response = curl_exec($ch2);
var_dump($seekCalls > 0);
var_dump(str_contains($response, $body));
?>
--EXPECT--
bool(true)
bool(true)
