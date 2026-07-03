--TEST--
SoapServer constructor with an invalid encoding option
--EXTENSIONS--
soap
--FILE--
<?php

class ExtendedSoapServer extends SoapServer {}

$wsdl = 'irrelevant';
$options = [
    'encoding' => 25,
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
TypeError: SoapServer::__construct(): Argument #2 ($options) "encoding" option must be of type string, int given
TypeError: SoapServer::__construct(): Argument #2 ($options) "encoding" option must be of type string, int given
