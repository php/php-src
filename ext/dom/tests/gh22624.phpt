--TEST--
GH-22624 (Use-after-free via DOMNameSpaceNode after DOMDocument::xinclude())
--CREDITS--
ExPatch-LLC
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (!function_exists('libxml_set_external_entity_loader')) die('skip xinclude not available');
?>
--FILE--
<?php
$included = __DIR__ . '/gh22624_included.xml';
file_put_contents($included, '<?xml version="1.0"?><included/>');
$href = 'file:///' . ltrim(str_replace('\\', '/', $included), '/');

$doc = new DOMDocument();
$doc->loadXML('<?xml version="1.0"?>
<root xmlns:xi="http://www.w3.org/2001/XInclude">
  <xi:include href="' . $href . '" xmlns:local="urn:test"/>
</root>');

$xpath = new DOMXPath($doc);
$xpath->registerNamespace('xi', 'http://www.w3.org/2001/XInclude');
$xi = $xpath->query('//xi:include')->item(0);
$ns = $xpath->query('namespace::local', $xi)->item(0); // DOMNameSpaceNode

$doc->xinclude(); // frees the xi:include element

var_dump($ns->parentNode);
var_dump($ns->parentElement);
var_dump($ns->isConnected);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh22624_included.xml');
?>
--EXPECT--
NULL
NULL
bool(false)
