--TEST--
CURLOPT_OPENSOCKETFUNCTION: clone, and changing the option from inside the callback
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start();

echo "clone carries the callback, which is then invoked with the clone\n";
$seen = [];
$ch = curl_init("{$host}/get.inc?test=method");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle) use (&$seen): bool {
    $seen[] = $handle;
    return true;
});
$clone = clone $ch;
var_dump(curl_exec($clone));
var_dump($seen !== [] && array_filter($seen, fn ($h) => $h !== $clone) === []);

echo "\nThe clone keeps refusing after the original's callback is removed\n";
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, null);
curl_setopt($clone, CURLOPT_OPENSOCKETFUNCTION, fn (): bool => false);
var_dump(curl_exec($clone), curl_errno($clone) === CURLE_COULDNT_CONNECT);
var_dump(curl_exec($ch));

echo "\nThe callback removes itself and allows the connection\n";
$calls = 0;
$ch = curl_init("{$host}/get.inc?test=method");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 5);
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle) use (&$calls): bool {
    $calls++;
    curl_setopt($handle, CURLOPT_OPENSOCKETFUNCTION, null);
    return true;
});
var_dump(curl_exec($ch), $calls);
var_dump(curl_exec($ch), $calls);

echo "Done\n";
?>
--EXPECT--
clone carries the callback, which is then invoked with the clone
string(3) "GET"
bool(true)

The clone keeps refusing after the original's callback is removed
bool(false)
bool(true)
string(3) "GET"

The callback removes itself and allows the connection
string(3) "GET"
int(1)
string(3) "GET"
int(1)
Done
