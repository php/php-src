--TEST--
Optional interfaces are rechecked on subsequent requests
--INI--
opcache.enable_cli=1
opcache.enable=1
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

$interfaceFile = __DIR__.'/optional_interfaces_interface.inc';

file_put_contents(__DIR__.'/optional_interfaces_script.php', <<<'SCRIPT'
<?php

$classFile = __DIR__.'/optional_interfaces_class.inc';
$interfaceFile = __DIR__.'/optional_interfaces_interface.inc';

if (opcache_is_script_cached($classFile))
    echo "Class is cached\n";
else
    echo "Class is not cached\n";

@(include $interfaceFile);

if (interface_exists('OpcachedInterface'))
    echo "OpcachedInterface is defined\n";
else
    echo "OpcachedInterface is not defined\n";

$hitsBefore = opcache_get_status()['opcache_statistics']['hits'];

include $classFile;

if (opcache_get_status()['opcache_statistics']['hits'] > $hitsBefore)
    echo "Class loaded from OPCache\n";
else
    echo "Class loaded from file\n";

echo "OpcachedClass implements:".implode(', ', class_implements('OpcachedClass'))."\n\n";
SCRIPT);

include __DIR__.'/php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d zend_extension=opcache');

$uri = 'http://' . PHP_CLI_SERVER_ADDRESS . '/optional_interfaces_script.php';

echo file_get_contents($uri);

file_put_contents($interfaceFile, '<?php interface OpcachedInterface {}');

echo file_get_contents($uri);

unlink($interfaceFile);

echo file_get_contents($uri);

unlink(__DIR__.'/optional_interfaces_script.php');

?>
--EXPECT--
Class is not cached
OpcachedInterface is not defined
Class loaded from file
OpcachedClass implements:

Class is cached
OpcachedInterface is defined
Class loaded from OPCache
OpcachedClass implements:OpcachedInterface

Class is cached
OpcachedInterface is not defined
Class loaded from OPCache
OpcachedClass implements:
