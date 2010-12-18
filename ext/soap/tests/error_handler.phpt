--TEST--
Bug #46760 (SoapClient doRequest fails when proxy is used)
--SKIPIF--
<?php require_once('skipif.inc'); 
if (!extension_loaded('sqlite')) die('skip squilite extension not available');
?>
--FILE--
<?php
echo "blllllllllaaaaaaa\n";
$var475 = sqlite_factory("\x00");
$var147 = use_soap_error_handler();
$var477 = flock(false,false);dump($client->_proxy_port);

echo "OK";
?>
--EXPECT--
OK
