--TEST--
SoapClient and SoapServer report an invalid classmap option as ValueError
--EXTENSIONS--
soap
--FILE--
<?php

$classmap = ['type' => 'stdClass', 1 => 'stdClass'];

try {
    new SoapClient(null, [
        'location' => 'http://example.com/',
        'uri' => 'urn:test',
        'classmap' => $classmap,
    ]);
} catch (Throwable $e) {
    echo 'SoapClient: ', $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    new SoapServer(null, [
        'uri' => 'urn:test',
        'classmap' => $classmap,
    ]);
} catch (Throwable $e) {
    echo 'SoapServer: ', $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
SoapClient: ValueError: SoapClient::__construct(): Argument #2 ($options) "classmap" option must be an associative array
SoapServer: ValueError: SoapServer::__construct(): Argument #2 ($options) "classmap" option must be an associative array
