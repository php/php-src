--TEST--
DOMNameSpaceNode clone after xinclude does not use a dangling parent
--EXTENSIONS--
dom
--FILE--
<?php
$included = __DIR__ . '/dom_namespacenode_clone_xinclude_included.xml';
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
$ns = $xpath->query('namespace::local', $xi)->item(0);

$live = clone $ns;
echo "live clone: ", $live->nodeName, "\n";
echo "live parent: ", $live->parentNode->nodeName, "\n";

$doc->xinclude();

$clone = clone $ns;
echo "after xinclude: ", $clone->nodeName, "\n";
var_dump($clone->parentNode);
var_dump($clone->parentElement);
var_dump($clone->isConnected);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/dom_namespacenode_clone_xinclude_included.xml');
?>
--EXPECT--
live clone: xmlns:local
live parent: xi:include
after xinclude: xmlns:local
NULL
NULL
bool(false)
