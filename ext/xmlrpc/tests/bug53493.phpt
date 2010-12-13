--TEST--
Bug #53493 (xmlrpc_decode should not be sensitive to leading whitespace)
--FILE--
<?php
$req = 		PHP_EOL . 
		'<?xml version="1.0"?><methodResponse><params><param>' . 
		'<value><string>Hello World</string></value></param>' . 
		'</params></methodResponse>';

var_dump(xmlrpc_decode($req));
echo "Done\n";
?>
--EXPECT--
string(11) "Hello World"
Done
