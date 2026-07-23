--TEST--
CURLOPT_SEEKFUNCTION is called to rewind a streamed upload across a redirect
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$body = 'Hello cURL seek!';
$offset = 0;
$seekCalls = 0;
$argsChecked = false;

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
curl_setopt($ch, CURLOPT_SEEKFUNCTION, function ($ch, int $position, int $origin) use (&$offset, &$seekCalls, &$argsChecked) {
    if (!$argsChecked) {
        $argsChecked = true;
        var_dump($ch instanceof CurlHandle);
        var_dump($position === 0);
        var_dump($origin === SEEK_SET);
    }
    if ($origin !== SEEK_SET) {
        return CURL_SEEKFUNC_CANTSEEK;
    }
    $seekCalls++;
    $offset = $position;
    return CURL_SEEKFUNC_OK;
});

$response = curl_exec($ch);
// The seek callback must have been invoked to rewind the body for the resend,
// and the resent body must have reached the redirect target intact.
var_dump($seekCalls > 0);
var_dump(str_contains($response, $body));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
