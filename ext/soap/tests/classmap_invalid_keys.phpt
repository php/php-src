--TEST--
SoapClient and SoapServer classmap options must only contain string keys
--EXTENSIONS--
soap
--FILE--
<?php

$emptyHash = ['type' => 'stdClass'];
unset($emptyHash['type']);

$cases = [
    'empty' => [],
    'empty hash' => $emptyHash,
    'packed' => ['stdClass'],
    'sparse numeric' => [100 => 'stdClass'],
    'numeric string' => ['1' => 'stdClass'],
    'mixed' => ['type' => 'stdClass', 1 => 'stdClass'],
    'associative' => ['type' => 'stdClass'],
];

foreach ($cases as $name => $classmap) {
    echo "-- $name --\n";

    try {
        new SoapClient(null, [
            'location' => 'http://example.com/',
            'uri' => 'urn:test',
            'classmap' => $classmap,
        ]);
        echo "SoapClient: OK\n";
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }

    try {
        new SoapServer(null, [
            'uri' => 'urn:test',
            'classmap' => $classmap,
        ]);
        echo "SoapServer: OK\n";
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
-- empty --
SoapClient: OK
SoapServer: OK
-- empty hash --
SoapClient: OK
SoapServer: OK
-- packed --
ValueError: SoapClient::__construct(): Argument #2 ($options) "classmap" option must be an associative array
ValueError: SoapServer::__construct(): Argument #2 ($options) "classmap" option must be an associative array
-- sparse numeric --
ValueError: SoapClient::__construct(): Argument #2 ($options) "classmap" option must be an associative array
ValueError: SoapServer::__construct(): Argument #2 ($options) "classmap" option must be an associative array
-- numeric string --
ValueError: SoapClient::__construct(): Argument #2 ($options) "classmap" option must be an associative array
ValueError: SoapServer::__construct(): Argument #2 ($options) "classmap" option must be an associative array
-- mixed --
ValueError: SoapClient::__construct(): Argument #2 ($options) "classmap" option must be an associative array
ValueError: SoapServer::__construct(): Argument #2 ($options) "classmap" option must be an associative array
-- associative --
SoapClient: OK
SoapServer: OK
