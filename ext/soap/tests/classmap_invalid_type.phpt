--TEST--
SoapClient and SoapServer classmap options must be arrays
--EXTENSIONS--
soap
--FILE--
<?php

try {
    new SoapClient(null, [
        'location' => 'http://example.com/',
        'uri' => 'urn:test',
        'classmap' => 1,
    ]);
} catch (Throwable $e) {
    echo 'SoapClient: ', $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    new SoapServer(null, [
        'uri' => 'urn:test',
        'classmap' => 1,
    ]);
} catch (Throwable $e) {
    echo 'SoapServer: ', $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
SoapClient: TypeError: SoapClient::__construct(): Argument #2 ($options) "classmap" option must be of type array, int given
SoapServer: TypeError: SoapServer::__construct(): Argument #2 ($options) "classmap" option must be of type array, int given
