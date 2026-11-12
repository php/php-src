--TEST--
CURLOPT_OPENSOCKETFUNCTION can refuse a redirect hop
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start();

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=redirect&target=method&code=302");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
/* Forbid reuse so that each hop opens a new connection. A reused connection
 * doesn't create any socket, so the callback doesn't see it. */
curl_setopt($ch, CURLOPT_FORBID_REUSE, true);

/* The callback can be called for both families on a dual stack host. The redirect
 * count tells which hop is checked. */
$hops = [];

echo "Allowing every hop\n";
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle) use (&$hops): bool {
    $hops[] = curl_getinfo($handle, CURLINFO_REDIRECT_COUNT) . ' ' . curl_getinfo($handle, CURLINFO_EFFECTIVE_URL);
    return true;
});
var_dump(curl_exec($ch));
print_r(array_values(array_unique($hops)));

echo "\nRefusing the redirect hop\n";
$hops = [];
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle) use (&$hops): bool {
    $redirects = curl_getinfo($handle, CURLINFO_REDIRECT_COUNT);
    $hops[] = $redirects . ' ' . curl_getinfo($handle, CURLINFO_EFFECTIVE_URL);
    return $redirects === 0;
});
var_dump(curl_exec($ch));
print_r(array_values(array_unique($hops)));
var_dump(curl_errno($ch) === CURLE_COULDNT_CONNECT);

echo "Done";
?>
--EXPECTF--
Allowing every hop
string(3) "GET"
Array
(
    [0] => 0 http://%s/get.inc?test=redirect&target=method&code=302
    [1] => 1 http://%s/get.inc?test=method
)

Refusing the redirect hop
bool(false)
Array
(
    [0] => 0 http://%s/get.inc?test=redirect&target=method&code=302
    [1] => 1 http://%s/get.inc?test=method
)
bool(true)
Done
