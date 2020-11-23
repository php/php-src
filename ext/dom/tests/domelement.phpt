--TEST--
Elements: DOMElement functionality
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML($xmlstr);
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

$node = $dom->documentElement;
echo "Tag Name: ".$node->tagName."\n";


$node->setAttribute('num', '1');
echo "Chapter: ".$node->getAttribute('num')."\n";
echo 'Attribute num exists?: '.($node->hasAttribute('num')?'Yes':'No')."\n";
$node->removeAttribute('num');
echo "Chapter: ".$node->getAttribute('num')."\n";
echo 'Attribute num exists?: '.($node->hasAttribute('num')?'Yes':'No')."\n";

echo "Language: ".$node->getAttribute('language')."\n";
$lang = $node->getAttributeNode('language');
$lang->nodeValue = 'en-US';
$node->setAttributeNode($lang);
echo "Language: ".$node->getAttribute('language')."\n";
$node->removeAttributeNode($lang);
echo "Language: ".$node->getAttribute('language')."\n";

echo "\n-- xml:lang --\n";
$node->setAttributeNS('http://www.w3.org/XML/1998/namespace', 'xml:lang', 'en');
echo "Language: ".$node->getAttributeNS('http://www.w3.org/XML/1998/namespace', 'lang')."\n";
echo 'Attribute xml:lang exists?: '.($node->hasAttributeNS('http://www.w3.org/XML/1998/namespace', 'lang')?'Yes':'No')."\n";

$node->removeAttributeNS('http://www.w3.org/XML/1998/namespace', 'lang');
echo "Language: ".$node->getAttributeNS('http://www.w3.org/XML/1998/namespace', 'lang')."\n";
echo 'Attribute xml:lang exists?: '.($node->hasAttributeNS('http://www.w3.org/XML/1998/namespace', 'lang')?'Yes':'No')."\n";

$lang = $dom->createAttributeNS('http://www.w3.org/XML/1998/namespace', 'xml:lang');
$lang->nodeValue = 'en-GB';
$node->setAttributeNodeNS($lang);
unset($lang);
echo "Language: ".$node->getAttributeNS('http://www.w3.org/XML/1998/namespace', 'lang')."\n";
$lang = $node->getAttributeNodeNS('http://www.w3.org/XML/1998/namespace', 'lang');
echo "Language: ".$lang->value."\n";

echo "\n-- Elements --\n";
$rows = $node->getElementsByTagName('row');
echo "Row Count: ".$rows->length."\n";

$element_ns = new DOMElement('newns:myelement', 'default content', 'urn::dummyns');
$node->appendChild($element_ns);
$element_ns = new DOMElement('newns2:myelement', 'second default content', 'urn::dummyns');
$node->appendChild($element_ns);

$myelements = $node->getElementsByTagNameNS('urn::dummyns', 'myelement');
$mylen = $myelements->length;
echo "myelements Count: ".$mylen."\n";

echo "\n-- IDs --\n";
$node->setAttribute('idatt', 'n1');
$node->setIdAttribute('idatt', TRUE);

for ($x = 0; $x < $mylen; $x++) {
    $current = $myelements->item($x);
    $current->setAttributeNS('urn::dummyns', 'newns:idatt', 'n'.($x+2))."\n";
    $current->setIdAttributeNS('urn::dummyns', 'idatt', TRUE);
}

echo 'Element Name: '.(($elem = $dom->getElementByID('n1'))?$elem->localName:'Not Found')."\n";
$idatt = $node->getAttributeNode('idatt');
$node->setIdAttributeNode($idatt, FALSE);
echo 'Element Name: '.(($elem = $dom->getElementByID('n1'))?$elem->localName:'Not Found')."\n";

echo 'Element Name: '.(($elem = $dom->getElementByID('n3'))?$elem->nodeName:'Not Found')."\n";
for ($x = 0; $x < $mylen; $x++) {
    $node = $myelements->item($x);
    $node->setIdAttributeNS('urn::dummyns', 'idatt', FALSE);
}
echo 'Element Name: '.(($elem = $dom->getElementByID('n3'))?$elem->nodeName:'Not Found')."\n";
?>
--EXPECT--
Tag Name: chapter
Chapter: 1
Attribute num exists?: Yes
Chapter: 
Attribute num exists?: No
Language: en
Language: en-US
Language: 

-- xml:lang --
Language: en
Attribute xml:lang exists?: Yes
Language: 
Attribute xml:lang exists?: No
Language: en-GB
Language: en-GB

-- Elements --
Row Count: 3
myelements Count: 2

-- IDs --
Element Name: chapter
Element Name: Not Found
Element Name: newns2:myelement
Element Name: Not Found
