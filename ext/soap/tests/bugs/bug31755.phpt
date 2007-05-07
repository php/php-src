--TEST--
Bug #31422 (No Error-Logging on SoapServer-Side)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$client=new SOAPClient(null, array('location' => 'http://localhost',
'uri' => 'myNS', 'exceptions' => false, 'trace' => true));

$header = new SOAPHeader(null, 'foo', 'bar');

$response= $client->__call('function', array(), null, $header);

print $client->__getLastRequest();
?>
--EXPECTF--
Fatal error: SoapHeader::SoapHeader(): Invalid parameters. Invalid namespace. in %s on line %d
