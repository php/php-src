--TEST--
Bug #42112 (deleting a node produces memory corruption)
--EXTENSIONS--
dom
--FILE--
<?php
$xml = <<<EOXML
<root><child xml:id="id1">baz</child></root>
EOXML;

function remove_node($doc) {
    $node = $doc->getElementById( 'id1' );
    print 'Deleting Node: '.$node->nodeName."\n";
    $node->parentNode->removeChild( $node );
}

$doc = new DOMDocument();
$doc->loadXML($xml);

remove_node($doc);

$node = $doc->getElementById( 'id1' );
if ($node) {
    print 'Found Node: '.$node->nodeName."\n";
}
$root = $doc->documentElement;
print 'Root Node: '.$root->nodeName."\n";
?>
--EXPECT--
Deleting Node: child
Root Node: root
