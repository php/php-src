--TEST--
SoapClient in non WSDL mode without required options
--EXTENSIONS--
soap
--FILE--
<?php

class ExtendedSoapClient extends SoapClient {}

echo "\$options not provided\n";
try {
    $client = new SoapClient(null);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $client = new ExtendedSoapClient(null);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Empty \$options array\n";
$options = [];
try {
    $client = new SoapClient(null, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $client = new ExtendedSoapClient(null, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "\$options array only sets \"uri\" option\n";
$options = ['uri' => 'https://example.com'];
try {
    $client = new SoapClient(null, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $client = new ExtendedSoapClient(null, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
$options not provided
SoapFault: SoapClient::__construct(): 'location' and 'uri' options are required in nonWSDL mode
SoapFault: SoapClient::__construct(): 'location' and 'uri' options are required in nonWSDL mode
Empty $options array
SoapFault: SoapClient::__construct(): 'uri' option is required in nonWSDL mode
SoapFault: SoapClient::__construct(): 'uri' option is required in nonWSDL mode
$options array only sets "uri" option
SoapFault: SoapClient::__construct(): 'location' option is required in nonWSDL mode
SoapFault: SoapClient::__construct(): 'location' option is required in nonWSDL mode
