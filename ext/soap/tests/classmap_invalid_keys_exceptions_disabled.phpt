--TEST--
SoapClient reports an invalid classmap option as ValueError when exceptions are disabled
--EXTENSIONS--
soap
--FILE--
<?php

try {
    new SoapClient(null, [
        'location' => 'http://example.com/',
        'uri' => 'urn:test',
        'exceptions' => false,
        'classmap' => ['type' => 'stdClass', 1 => 'stdClass'],
    ]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: SoapClient::__construct(): Argument #2 ($options) "classmap" option must be an associative array
