--TEST--
Optional interfaces are rechecked on subsequent requests
--INI--
opcache.enable_cli=1
opcache.enable=1
--EXTENSIONS--
opcache
--FILE--
<?php

$cmd = PHP_BINARY . " " . escapeshellarg(__DIR__.'/opcached_script.php');
$interfaceFile = __DIR__.'/opcached_interface.inc';

echo shell_exec($cmd);

file_put_contents($interfaceFile, '<?php interface OpcachedInterface {}');

echo shell_exec($cmd);

unlink($interfaceFile);

echo shell_exec($cmd);

?>
--EXPECT--
OpcachedInterface is not defined
OpcachedClass implements

OpcachedInterface is defined
OpcachedClass implements OpcachedInterface

OpcachedInterface is not defined
OpcachedClass implements
