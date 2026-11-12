--TEST--
CURLOPT_OPENSOCKETFUNCTION: IPv6 addresses
--EXTENSIONS--
curl
--SKIPIF--
<?php
$s = @stream_socket_server('tcp://[::1]:0', $errno, $errstr);
if ($s === false) die('skip no IPv6 loopback available');
fclose($s);
?>
--FILE--
<?php
/* No listener is needed: the callback runs before connect() */
function probe(string $url): void {
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle, string $address, int $port): bool {
        var_dump($address, $port);
        return false;
    });
    curl_exec($ch);
}

probe('http://[::1]:1/');
probe('http://[::ffff:127.0.0.1]:2/');
probe('http://[::ffff:7f00:1]:3/');

echo "Done";
?>
--EXPECT--
string(3) "::1"
int(1)
string(16) "::ffff:127.0.0.1"
int(2)
string(16) "::ffff:127.0.0.1"
int(3)
Done
