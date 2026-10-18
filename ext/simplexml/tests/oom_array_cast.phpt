--TEST--
Array cast of large tree doesn't reach hard memory limit
--EXTENSIONS--
simplexml
--INI--
memory_limit=64M
--FILE--
<?php
$path = __DIR__ . '/oom_array_cast.xml';
file_put_contents($path, '<r>' . str_repeat('<a/>', 750000) . '</r>');

$xml = simplexml_load_file($path);
$result = (array) $xml;
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_array_cast.xml');
?>
--EXPECTF--
Fatal error: Allowed memory size of 67108864 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
