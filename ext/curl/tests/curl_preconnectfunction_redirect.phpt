--TEST--
CURLOPT_PRECONNECTFUNCTION can refuse a redirect hop
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
/* Forbid reuse so each hop opens a new connection and is therefore vetted. A
 * pooled connection creates no socket and is not seen by the callback at all. */
curl_setopt($ch, CURLOPT_FORBID_REUSE, true);

/* libcurl calls the callback once per socket, so a dual stack host may be
 * attempted for several address families. The redirect count identifies the hop. */
$hops = [];

echo "Allowing every hop\n";
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function (CurlHandle $handle) use (&$hops): bool {
    $hops[] = curl_getinfo($handle, CURLINFO_REDIRECT_COUNT) . ' ' . curl_getinfo($handle, CURLINFO_EFFECTIVE_URL);
    return true;
});
var_dump(curl_exec($ch));
print_r(array_values(array_unique($hops)));

echo "\nRefusing the redirect hop\n";
$hops = [];
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function (CurlHandle $handle) use (&$hops): bool {
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
