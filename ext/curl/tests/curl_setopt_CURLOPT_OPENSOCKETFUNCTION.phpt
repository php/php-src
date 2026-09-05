--TEST--
Curl option CURLOPT_OPENSOCKETFUNCTION
--EXTENSIONS--
curl
sockets
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();
$port = (int) explode(':', $host)[1];

$url = "{$host}/get.inc";

$seen = null;
$allow = true;
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function ($ch, $purpose, $address) use (&$seen, &$allow) {
    $seen ??= [
        'handle' => $ch instanceof CurlHandle,
        'purpose' => $purpose === CURLSOCKTYPE_IPCXN,
        'family_is_int' => is_int($address['family']),
        'socktype_is_int' => is_int($address['socktype']),
        'protocol_is_int' => is_int($address['protocol']),
        'ip_is_valid' => inet_pton($address['ip']) !== false,
        'port' => $address['port'],
    ];
    if (!$allow) {
        return false;
    }
    // Userland creates the socket libcurl then connects through.
    return socket_create($address['family'], $address['socktype'], $address['protocol']);
});

echo "Testing with an allowed connection\n";
var_dump(curl_exec($ch));
var_dump(curl_errno($ch));
var_dump($seen['handle'], $seen['purpose'], $seen['family_is_int'],
    $seen['socktype_is_int'], $seen['protocol_is_int'], $seen['ip_is_valid']);
var_dump($seen['port'] === $port);

echo "\nTesting with a blocked connection (return false)\n";
$allow = false;
var_dump(curl_exec($ch));
var_dump(curl_errno($ch) === CURLE_COULDNT_CONNECT);

echo "\nTesting with curl_copy_handle\n";
$allow = true;
$ch2 = curl_copy_handle($ch);
var_dump(curl_exec($ch2));

echo "\nTesting with invalid return type\n";
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function ($ch, $purpose, $address) {
    return 42;
});
try {
    curl_exec($ch);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "\nTesting with invalid option value\n";
try {
    curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, 42);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "\nTesting with null as the callback\n";
var_dump(curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, null));
var_dump(curl_exec($ch));
var_dump(curl_errno($ch));

echo "\nTesting with an already-closed Socket return\n";
// Returning a Socket whose descriptor has been closed must be refused with a
// clear TypeError rather than handed to libcurl, which would otherwise bury
// the real cause under a generic CURLE_COULDNT_CONNECT.
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function ($ch, $purpose, $address) {
    $s = socket_create($address['family'], $address['socktype'], $address['protocol']);
    socket_close($s);
    return $s;
});
try {
    curl_exec($ch);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "\nTesting with a stream-backed Socket return\n";
// A Socket obtained from socket_import_stream() shares its descriptor with a
// php_stream, which owns close lifecycle in socket_free_obj(). Letting libcurl
// take that descriptor would result in a double close, so the extension
// refuses it explicitly rather than silently corrupting state.
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function ($ch, $purpose, $address) {
    // We only need a stream-backed Socket; a listening socket on a free port
    // is the simplest portable way to get one (works identically on Linux,
    // FreeBSD, macOS and Windows).
    $server = stream_socket_server('tcp://127.0.0.1:0');
    return socket_import_stream($server);
});
try {
    curl_exec($ch);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "\nTesting with a callback that throws\n";
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function ($ch, $purpose, $address) {
    throw new RuntimeException('opensocket boom');
});
try {
    curl_exec($ch);
    echo "FAIL: curl_exec did not propagate the exception\n";
} catch (\RuntimeException $e) {
    echo "caught: ", $e->getMessage(), \PHP_EOL;
}

echo "\nDone";
?>
--EXPECT--
Testing with an allowed connection
string(25) "Hello World!
Hello World!"
int(0)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Testing with a blocked connection (return false)
bool(false)
bool(true)

Testing with curl_copy_handle
string(25) "Hello World!
Hello World!"

Testing with invalid return type
The CURLOPT_OPENSOCKETFUNCTION callback must return a Socket or false

Testing with invalid option value
curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_OPENSOCKETFUNCTION, no array or string given

Testing with null as the callback
bool(true)
string(25) "Hello World!
Hello World!"
int(0)

Testing with an already-closed Socket return
The CURLOPT_OPENSOCKETFUNCTION callback must return an open Socket

Testing with a stream-backed Socket return
The CURLOPT_OPENSOCKETFUNCTION callback must return a Socket whose descriptor it owns; stream-backed Sockets (socket_import_stream) are not supported

Testing with a callback that throws
caught: opensocket boom

Done
