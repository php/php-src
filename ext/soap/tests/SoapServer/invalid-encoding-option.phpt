--TEST--
SoapServer constructor with an invalid encoding option
--EXTENSIONS--
soap
--FILE--
<?php

class ExtendedSoapServer extends SoapServer {}

$wsdl = 'irrelevant';
$options = [
    'encoding' => 'non-sense',
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
ValueError: SoapServer::__construct(): Argument #2 ($options) provided "encoding" option "non-sense" is invalid
ValueError: SoapServer::__construct(): Argument #2 ($options) provided "encoding" option "non-sense" is invalid
