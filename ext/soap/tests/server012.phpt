--TEST--
SOAP Server 12: WSDL generation
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--GET--
WSDL
--FILE--
<?php
function Add($x,$y) {
  return $x+$y;
}

$server = new soapserver("http://testuri.org");
$server->addfunction("Add");
$server->handle();
echo "ok\n";
?>
--EXPECT--
