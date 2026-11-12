--TEST--
CURLOPT_OPENSOCKETFUNCTION: refusing an address makes libcurl try the next one
--EXTENSIONS--
curl
--FILE--
<?php
/* No listener is needed: the callback runs before connect() */
function probe(callable $callback): void {
    $ch = curl_init('http://example.test:1/');
    curl_setopt($ch, CURLOPT_RESOLVE, ['example.test:1:127.0.0.2,127.0.0.3']);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, $callback);
    try {
        var_dump(curl_exec($ch));
    } catch (Exception $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    var_dump(curl_errno($ch) === CURLE_COULDNT_CONNECT);
}

echo "Refusing every address\n";
probe(function (CurlHandle $handle, string $address, int $port): bool {
    echo "$address:$port\n";
    return false;
});

echo "\nThrowing on the first address\n";
probe(function (CurlHandle $handle, string $address, int $port): bool {
    echo "$address:$port\n";
    throw new Exception('refused');
});

echo "Done";
?>
--EXPECT--
Refusing every address
127.0.0.2:1
127.0.0.3:1
bool(false)
bool(true)

Throwing on the first address
127.0.0.2:1
Exception: refused
bool(true)
Done
