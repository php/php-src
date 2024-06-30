--TEST--
SoapServer constructor with an invalid encoding option
--EXTENSIONS--
soap
--FILE--
<?php

class ExtendedSoapServer extends SoapServer {}

$wsdl = 'irrelevant';
$options = [
    'soap_version' => 1111,
];
try {
    $client = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $client = new ExtendedSoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}


?>
--EXPECT--
ValueError: SoapServer::__construct(): Argument #2 ($options) "soap_version" option must be SOAP_1_1 or SOAP_1_2
ValueError: SoapServer::__construct(): Argument #2 ($options) "soap_version" option must be SOAP_1_1 or SOAP_1_2
