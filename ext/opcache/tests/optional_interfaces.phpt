--TEST--
Optional interfaces are rechecked on subsequent requests
--INI--
opcache.enable_cli=1
opcache.enable=1
--EXTENSIONS--
opcache
--FILE--
<?php

$interfaceFile = __DIR__.'/optional_interfaces_interface.inc';

include __DIR__.'/php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1');

$uri = 'http://' . PHP_CLI_SERVER_ADDRESS . '/optional_interfaces_script.php';

echo file_get_contents($uri);

file_put_contents($interfaceFile, '<?php interface OpcachedInterface {}');

echo file_get_contents($uri);

unlink($interfaceFile);

echo file_get_contents($uri);

?>
--EXPECT--
OpcachedInterface is not defined
OpcachedClass implements:

OpcachedInterface is defined
OpcachedClass implements:OpcachedInterface

OpcachedInterface is not defined
OpcachedClass implements:
