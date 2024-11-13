--TEST--
GH-11941 (soap with session persistence will silently fail when "session" built as a shared object)
--EXTENSIONS--
soap
--SKIPIF--
<?php
// We explicitly want to test with the soap extension enabled and session extension disabled
if (extension_loaded("session")) die("skip this test must run with the session extension disabled");
?>
--FILE--
<?php
$server = new SoapServer(null, array('uri'=>"http://testuri.org"));
$server->setPersistence(SOAP_PERSISTENCE_SESSION);
?>
--EXPECTF--
%aUncaught Error: SoapServer::setPersistence(): Persistence cannot be set when the SOAP server is used in function mode in %s:%d
%a
