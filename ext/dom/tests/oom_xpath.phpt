--TEST--
DOMXPath::query() with large result set doesn't reach hard memory limit
--EXTENSIONS--
dom
--INI--
memory_limit=64M
--FILE--
<?php
$path = __DIR__ . '/oom_xpath.xml';
file_put_contents($path, '<r>' . str_repeat('<a/>', 1000000) . '</r>');

$xml = new DOMDocument;
$xml->load($path);
$result = (new DOMXPath($xml))->query("//a");
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_xpath.xml');
?>
--EXPECTF--
Fatal error: Allowed memory size of 67108864 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d

