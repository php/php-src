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
ValueError: SoapClient::__construct(): Argument #2 ($options) provided "encoding" option "non-sense" is invalid
ValueError: SoapClient::__construct(): Argument #2 ($options) provided "encoding" option "non-sense" is invalid
