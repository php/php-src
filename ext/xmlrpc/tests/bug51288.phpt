--TEST--
Bug #51288 (CVE-2010-0397, NULL pointer deref when no <methodName> in request)
--FILE--
<?php
$method = NULL;
$req = '<?xml version="1.0"?><methodCall></methodCall>';
var_dump(xmlrpc_decode_request($req, $method));
var_dump($method);
echo "Done\n";
?>
--EXPECT--
NULL
NULL
Done
