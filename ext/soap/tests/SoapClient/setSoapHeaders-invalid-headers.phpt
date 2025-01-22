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
    $client->__setSoapHeaders($headers);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
Fatal error: SoapClient::__setSoapHeaders(): Invalid SOAP header in %s on line %d
