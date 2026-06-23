--TEST--
CURLOPT_SEEKFUNCTION callback error handling and option validation
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

// Drive a 307-redirect upload so libcurl invokes the seek callback to rewind
// the body; $seek is the callback under test.
function run_upload(string $host, callable $seek): void
{
    $offset = 0;
    $body = 'Hello cURL seek!';
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
    curl_setopt($ch, CURLOPT_SEEKFUNCTION, $seek);
    curl_exec($ch);
}

echo "Returning a non-int:\n";
try {
    run_upload($host, fn($ch, $offset, $origin) => 'not an int');
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "\nReturning an out-of-range int:\n";
try {
    run_upload($host, fn($ch, $offset, $origin) => 42);
} catch (\ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "\nThrowing from the callback:\n";
try {
    run_upload($host, function ($ch, $offset, $origin) {
        throw new \RuntimeException('boom from seek');
    });
} catch (\RuntimeException $e) {
    echo $e->getMessage(), "\n";
}

echo "\nSetting the callback to null:\n";
var_dump(curl_setopt(curl_init(), CURLOPT_SEEKFUNCTION, null));

echo "\nSetting a non-callable scalar:\n";
try {
    curl_setopt(curl_init(), CURLOPT_SEEKFUNCTION, 42);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Returning a non-int:
The CURLOPT_SEEKFUNCTION callback must return one of CURL_SEEKFUNC_OK, CURL_SEEKFUNC_FAIL or CURL_SEEKFUNC_CANTSEEK

Returning an out-of-range int:
The CURLOPT_SEEKFUNCTION callback must return one of CURL_SEEKFUNC_OK, CURL_SEEKFUNC_FAIL or CURL_SEEKFUNC_CANTSEEK

Throwing from the callback:
boom from seek

Setting the callback to null:
bool(true)

Setting a non-callable scalar:
curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_SEEKFUNCTION, no array or string given
