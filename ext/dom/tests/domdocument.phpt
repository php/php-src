--TEST--
CharData: DOMCharacterData and related functionality
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument("1.0", "UTF-8");
$root = $dom->createElement('root');
$dom->appendChild($root);

$dom2 = new DOMDocument("1.0");
$dom2->loadXML(b'<doc2><child1>data</child1></doc2>');
$root2 = $dom2->documentElement;

$imported = $dom->importNode($root2->firstChild, TRUE);

$root->appendChild($imported);

$att = $dom->createAttribute('imported');
$attval = $dom->createTextNode('1');
$att->appendChild($attval);
$imported->setAttributeNode($att);

$comment = $dom->createComment('testing DOMDocument functionality');
$dom->insertBefore($comment, $root);

for ($x=1; $x < 3; $x++) {
	if ($x == 2) {
		$node = $dom->createElementNS('urn::foo', 'foo:child', 'data ');
		$entity = $dom->createEntityReference('amp');
		$node->appendChild($entity);
		$node->appendChild(new DOMText(' stuff'));
	} else {
		$node = $dom->createElementNS('urn::foo', 'foo:child');
		$data = $dom->createCDATASection('cdata');
		$node->appendChild($data);
	}
	$root->appendChild($node);

	$att = $dom->createAttributeNS('urn::foo', 'foo:bar');
	$att->appendChild($dom->createTextNode($x));
	$node->setAttributeNode($att);
	$att = $dom->createAttribute('xml:id');
	$att->appendChild($dom->createTextNode('id'.$x));
	$node->setAttributeNode($att);
}

$nodes = $dom->getElementsByTagName('child1');
$node = $nodes->item(0);
$value = $dom->createProcessingInstruction('php', "echo 'Hello World'; ");
$node->replaceChild($value, $node->firstChild);

$nodes = $dom->getElementsByTagNameNS('urn::foo', 'child');
$node = $nodes->item(1);
$frag = $dom->createDocumentFragment();
$frag->appendChild($node);

var_dump($dom->saveXML($frag));

$file = dirname(__FILE__).'/dom.tmp';
$dom->save($file);

$doc = new DOMDocument();
$doc->load($file);

$node = $doc->getElementById('id1');
$node->parentNode->removeChild($node);

print "\n".$doc->saveXML();

unlink($file);

$file = dirname(__FILE__).'/html.tmp';
$doc->loadHTMLFile(dirname(__FILE__).'/test.html');
$doc->saveHTMLFile($file);
$output = $doc->saveHTML();
$dom->loadHTMLFile($file);
$output2 = $dom->saveHTML();

if ($output === $output2) {
	print "\nOutput is identical\n";
} else {
	print "\nOutput is different\n";
}

unlink($file);
?>
--EXPECT--

string(85) "<foo:child xmlns:foo="urn::foo" foo:bar="2" xml:id="id2">data &amp; stuff</foo:child>"

<?xml version="1.0" encoding="UTF-8"?>
<!--testing DOMDocument functionality-->
<root xmlns:foo="urn::foo"><child1 imported="1"><?php echo 'Hello World'; ?></child1></root>

Output is identical
