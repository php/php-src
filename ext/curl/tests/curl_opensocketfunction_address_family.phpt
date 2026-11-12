--TEST--
CURLOPT_OPENSOCKETFUNCTION reports the address and port of IPv4 and IPv6 endpoints
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!(curl_version()['features'] & CURL_VERSION_IPV6)) die('skip libcurl built without IPv6');
/* The IPv6 probes below rely on libcurl attempting a literal ::1 address, which
 * it will not do on a host without a usable IPv6 loopback. */
$s = @stream_socket_server('tcp://[::1]:0', $errno, $errstr);
if ($s === false) die('skip no IPv6 loopback available');
fclose($s);
?>
--FILE--
<?php
/* No listener is needed: the callback runs before connect(). Proxies from the
 * environment are ignored, or the callback would see the proxy instead. */
function probe(string $url): void {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_PROXY, '');
    curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle, string $address, int $port): bool {
        var_dump($address, $port);
        return false;
    });
    curl_exec($ch);
}

echo "IPv4 literal\n";
probe('http://127.0.0.1:1/');

echo "\nIPv6 literal\n";
probe('http://[::1]:1/');

/* An IPv4-mapped IPv6 address reaches IPv4 from an AF_INET6 socket. Both forms
 * normalise to the same string. */
echo "\nIPv4-mapped IPv6 address\n";
probe('http://[::ffff:127.0.0.1]:1/');

echo "\nIPv4-mapped IPv6 address, hexadecimal form\n";
probe('http://[::ffff:7f00:1]:1/');

echo "Done";
?>
--EXPECT--
IPv4 literal
string(9) "127.0.0.1"
int(1)

IPv6 literal
string(3) "::1"
int(1)

IPv4-mapped IPv6 address
string(16) "::ffff:127.0.0.1"
int(1)

IPv4-mapped IPv6 address, hexadecimal form
string(16) "::ffff:127.0.0.1"
int(1)
Done
