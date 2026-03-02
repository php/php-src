--TEST--
SOAP Server: SoapServer::setObject twice
--EXTENSIONS--
soap
--FILE--
<?php
$foo = new stdClass();
$server = new SoapServer(null,array('uri'=>"http://testuri.org"));
$server->setObject($foo);
$server->setObject($foo);
echo "Done\n";
?>
--EXPECT--
Done
