--TEST--
SoapClient reports an invalid classmap option as a fatal error when exceptions are disabled
--EXTENSIONS--
soap
--FILE--
<?php

new SoapClient(null, [
    'location' => 'http://example.com/',
    'uri' => 'urn:test',
    'exceptions' => false,
    'classmap' => ['type' => 'stdClass', 1 => 'stdClass'],
]);
echo 'not reached', PHP_EOL;

?>
--EXPECTF--
Fatal error: SoapClient::__construct(): 'classmap' option must be an associative array in %s on line %d
