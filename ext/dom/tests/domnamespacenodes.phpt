--TEST--
Namespace nodes: DOMNamespaceNode functionality
--EXTENSIONS--
dom
--FILE--
<?php
$xmlStr = <<<EOXML
<hello xmlns="http://www.hello.com" xmlns:foo="http://foo">
    <item xmlns:bar="http://bar">A</item>
</hello>
EOXML;

$dom = new DOMDocument;
$dom->loadXML($xmlStr);
if(!$dom) {
    echo "Error while parsing the document\n";
    exit;
}

$node = $dom->documentElement;
$checkInstance = fn ($node): string =>
	($node instanceof DOMNode && $node instanceof DOMNameSpaceNode) ? 'YES' : 'NO';


$xmlns = $node->getAttributeNode('xmlns');
echo "Root XMLNS uri: ".$xmlns->namespaceURI."\n";
echo "Root XMLNS prefix: ".$xmlns->prefix."\n";
echo "This is a node: " . $checkInstance($xmlns) . "\n";

$foo = $node->getAttributeNode('xmlns:foo');
echo "Root XMLNS:foo uri: ".$foo->namespaceURI."\n";
echo "Root XMLNS:foo prefix: ".$foo->prefix."\n";
echo "This is a node: " . $checkInstance($foo) . "\n";

$bar = $node->firstElementChild->getAttributeNode('xmlns:bar');
echo "Item XMLNS:bar uri: ".$bar->namespaceURI."\n";
echo "Item XMLNS:bar prefix: ".$bar->prefix."\n";
echo "This is a node: " . $checkInstance($bar) . "\n";

$node->firstElementChild->removeAttributeNS($bar->namespaceURI, $bar->prefix);

print $dom->saveXML($node);

?>
--EXPECT--
Root XMLNS uri: http://www.hello.com
Root XMLNS prefix: 
This is a node: YES
Root XMLNS:foo uri: http://foo
Root XMLNS:foo prefix: foo
This is a node: YES
Item XMLNS:bar uri: http://bar
Item XMLNS:bar prefix: bar
This is a node: YES
<hello xmlns="http://www.hello.com" xmlns:foo="http://foo">
    <item>A</item>
</hello>
