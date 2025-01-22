--TEST--
SoapClient constructor with an invalid encoding option
--EXTENSIONS--
soap
--FILE--
<?php

class ExtendedSoapClient extends SoapClient {}

$wsdl = 'irrelevant';
$options = [
    'encoding' => 'non-sense',
];
try {
    $client = new SoapClient($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $client = new ExtendedSoapClient($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}


?>
--EXPECT--
SoapFault: SoapClient::__construct(): Invalid 'encoding' option - 'non-sense'
SoapFault: SoapClient::__construct(): Invalid 'encoding' option - 'non-sense'
