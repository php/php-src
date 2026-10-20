--TEST--
SoapServer constructor invalid type errors
--EXTENSIONS--
soap
--FILE--
<?php

$wsdl = 'irrelevant';

$options = [
    'uri' => 25,
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'actor' => 25,
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'classmap' => 25,
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'typemap' => 25,
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'features' => 'not an int',
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'cache_wsdl' => 'not an int',
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'send_errors' => 'not an int or bool',
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$options = [
    'trace' => 'not an int or bool',
];
try {
    $server = new SoapServer($wsdl, $options);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: SoapServer::__construct(): Argument #2 ($options) "uri" option must be of type string, int given
TypeError: SoapServer::__construct(): Argument #2 ($options) "actor" option must be of type string, int given
TypeError: SoapServer::__construct(): Argument #2 ($options) "classmap" option must be of type array, int given
TypeError: SoapServer::__construct(): Argument #2 ($options) "typemap" option must be of type array, int given
TypeError: SoapServer::__construct(): Argument #2 ($options) "features" option must be of type int, string given
TypeError: SoapServer::__construct(): Argument #2 ($options) "cache_wsdl" option must be of type int, string given
TypeError: SoapServer::__construct(): Argument #2 ($options) "send_errors" option must be of type bool, string given
TypeError: SoapServer::__construct(): Argument #2 ($options) "trace" option must be of type bool, string given
