--TEST--
SOAP Server 11: bind
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--GET--
wsdl
--FILE--
<?php
function Add($x,$y) {
  return $x+$y;
}

$server = new soapserver(dirname(__FILE__)."/test.wsdl");
ob_start();
$server->handle();
$wsdl = ob_get_contents();
ob_end_clean();
if ($wsdl == file_get_contents(dirname(__FILE__)."/test.wsdl")) {
  echo "ok\n";
} else {
	echo "fail\n";
}
?>
--EXPECT--
ok
