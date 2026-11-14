--TEST--
SoapClient constructor option errors are fatal errors with the "exceptions" option disabled
--EXTENSIONS--
soap
--FILE--
<?php
new SoapClient(null, ['location' => 'http://127.0.0.1:9/', 'uri' => 'urn:x', 'exceptions' => false, 'encoding' => 'no-such-encoding']);
echo "not reached\n";
?>
--EXPECTF--
Fatal error: SoapClient::__construct(): Invalid 'encoding' option - 'no-such-encoding' in %s on line %d
