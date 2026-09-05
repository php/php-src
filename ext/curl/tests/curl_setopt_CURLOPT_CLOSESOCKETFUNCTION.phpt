--TEST--
Curl option CURLOPT_CLOSESOCKETFUNCTION
--EXTENSIONS--
curl
sockets
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$url = "{$host}/get.inc";

$seen = null;
$closeCount = 0;
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
// Force the connection (and therefore the socket close) to happen during the
// transfer rather than at handle destruction, so the callback is observable.
curl_setopt($ch, CURLOPT_FORBID_REUSE, 1);
curl_setopt($ch, CURLOPT_CLOSESOCKETFUNCTION, function ($ch, $socket) use (&$seen, &$closeCount) {
    $closeCount++;
    $seen ??= [
        'handle' => $ch instanceof CurlHandle,
        'socket' => $socket instanceof Socket,
    ];
    socket_close($socket);
});

echo "Testing close-socket callback\n";
var_dump(curl_exec($ch));
var_dump(curl_errno($ch));
var_dump($seen);
var_dump($closeCount >= 1);

echo "\nTesting with invalid option value\n";
try {
    curl_setopt($ch, CURLOPT_CLOSESOCKETFUNCTION, 42);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "\nTesting with null as the callback\n";
var_dump(curl_setopt($ch, CURLOPT_CLOSESOCKETFUNCTION, null));
var_dump(curl_exec($ch));
var_dump(curl_errno($ch));

echo "\nTesting with a callback that throws\n";
// When libcurl closes the socket within curl_easy_perform() (HTTP/1.0 or
// CURLOPT_FORBID_REUSE), an exception thrown from the callback propagates
// out of curl_exec() and can be caught. If the socket is closed later at
// curl_easy_cleanup() instead, the extension tears down the close FCC first
// so the callback is not invoked from a destructor.
$ch2 = curl_init();
curl_setopt($ch2, CURLOPT_URL, $url);
curl_setopt($ch2, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch2, CURLOPT_FORBID_REUSE, 1);
curl_setopt($ch2, CURLOPT_CLOSESOCKETFUNCTION, function ($ch, $socket) {
    throw new RuntimeException('close boom');
});
try {
    curl_exec($ch2);
    echo "FAIL: curl_exec did not propagate the exception\n";
} catch (\RuntimeException $e) {
    echo "caught: ", $e->getMessage(), \PHP_EOL;
}

echo "\nDone";
?>
--EXPECT--
Testing close-socket callback
string(25) "Hello World!
Hello World!"
int(0)
array(2) {
  ["handle"]=>
  bool(true)
  ["socket"]=>
  bool(true)
}
bool(true)

Testing with invalid option value
curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_CLOSESOCKETFUNCTION, no array or string given

Testing with null as the callback
bool(true)
string(25) "Hello World!
Hello World!"
int(0)

Testing with a callback that throws
caught: close boom

Done
