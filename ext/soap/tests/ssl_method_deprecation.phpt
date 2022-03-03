--TEST--
ssl_method option is deprecated
--EXTENSIONS--
soap
--FILE--
<?php

new SoapClient(null, [
    'location' => 'foo',
    'uri' => 'bar',
    'ssl_method' => SOAP_SSL_METHOD_TLS,
]);

?>
--EXPECTF--
Deprecated: SoapClient::__construct(): The "ssl_method" option is deprecated. Use "ssl" stream context options instead in %s on line %d
