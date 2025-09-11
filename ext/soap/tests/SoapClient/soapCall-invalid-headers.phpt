--TEST--
SoapClient::__soapCall with invalid headers
--EXTENSIONS--
soap
--FILE--
<?php

/* Bypass constructor */
class ExtendedSoapClient extends SoapClient {
    public function __construct() {}
}

$client = new ExtendedSoapClient();
$header = new SoapHeader('namespace', 'name');
$headers = [
    $header,
    'giberrish',
];

try {
    $client->__soapCall('function', ['arg'], ['options'], $headers);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: SoapClient::__soapCall(): Argument #4 ($inputHeaders) must be on array of SoapHeader objects
