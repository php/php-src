--TEST--
SoapServer in non WSDL mode without required options
--EXTENSIONS--
soap
--FILE--
<?php

class ExtendedSoapServer extends SoapServer {}

echo "\$options not provided\n";
try {
    $client = new SoapServer(null);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $client = new ExtendedSoapServer(null);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Empty \$options array\n";
$options = [];
try {
    $client = new SoapServer(null, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $client = new ExtendedSoapServer(null, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
$options not provided
ValueError: SoapServer::__construct(): Argument #2 ($options) must provide "uri" option as it is required in nonWSDL mode
ValueError: SoapServer::__construct(): Argument #2 ($options) must provide "uri" option as it is required in nonWSDL mode
Empty $options array
ValueError: SoapServer::__construct(): Argument #2 ($options) must provide "uri" option as it is required in nonWSDL mode
ValueError: SoapServer::__construct(): Argument #2 ($options) must provide "uri" option as it is required in nonWSDL mode
