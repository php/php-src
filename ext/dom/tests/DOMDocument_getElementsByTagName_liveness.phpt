--TEST--
DOMDocument::getElementsByTagName() is live
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML( '<root><e i="1"/><e i="2"/><e i="3"/><e i="4"/><e i="5"/><e i="6"/><e i="7"/><e i="8"/><e i="9"/><e i="10"/><e i="11"/><e i="12"/><e i="13"/><e i="14"/><e i="15"/></root>' );
$root = $doc->documentElement;

$i = 0;

foreach ($doc->getElementsByTagName('e') as $node) {
	print $node->getAttribute('i') . ' ';
	if ($i++ % 2 == 0)
		$root->removeChild($node);
}
print "\n";
?>
--EXPECT--
1 3 4 6 7 9 10 12 13 15
