--TEST--
CURLOPT_PRECONNECTFUNCTION reports the address family of the endpoint
--EXTENSIONS--
curl
--SKIPIF--
<?php
/* The IPv6 probes below rely on libcurl attempting a literal ::1 address, which
 * it will not do on a host without a usable IPv6 loopback. */
$s = @stream_socket_server('tcp://[::1]:0', $errno, $errstr);
if ($s === false) die('skip no IPv6 loopback available');
fclose($s);
?>
--FILE--
<?php
/* No listener is needed: the callback runs before connect(). */
function probe(string $url): void {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function (CurlHandle $handle, ?string $ip, int $port, CurlAddressFamily $family): bool {
        var_dump($ip, $port, $family);
        return false;
    });
    curl_exec($ch);
}

echo "IPv4 literal\n";
probe('http://127.0.0.1:1/');

echo "\nIPv6 literal\n";
probe('http://[::1]:1/');

/* An IPv4-mapped IPv6 address reaches IPv4 from an AF_INET6 socket, so the family
 * is Inet6 while the reachable host is IPv4. Both forms normalise to the same
 * string. A policy has to look at $ip, not only at $family. */
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
enum(CurlAddressFamily::Inet)

IPv6 literal
string(3) "::1"
int(1)
enum(CurlAddressFamily::Inet6)

IPv4-mapped IPv6 address
string(16) "::ffff:127.0.0.1"
int(1)
enum(CurlAddressFamily::Inet6)

IPv4-mapped IPv6 address, hexadecimal form
string(16) "::ffff:127.0.0.1"
int(1)
enum(CurlAddressFamily::Inet6)
Done
