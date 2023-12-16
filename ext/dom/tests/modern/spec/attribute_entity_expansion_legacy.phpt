--TEST--
Attribute entity expansion in a legacy document
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$elt = $doc->createElement('elt');
$doc->appendChild($elt);
$elt->setAttribute('a','&');
print $doc->saveXML($elt) . "\n";

$attr = $elt->getAttributeNode('a');
$attr->value = '&amp;';
print "$attr->value\n";
print $doc->saveXML($elt) . "\n";

$attr->removeChild($attr->firstChild);
print $doc->saveXML($elt) . "\n";

$attr->nodeValue = '&';
print "$attr->nodeValue\n";
print $doc->saveXML($elt) . "\n";

$attr->nodeValue = '&amp;';
print "$attr->nodeValue\n";
print $doc->saveXML($elt) . "\n";

$elt->removeAttributeNode($attr);
$elt->setAttributeNS('http://www.w3.org/2000/svg', 'svg:id','&amp;');
print $doc->saveXML($elt) . "\n";

$attr = $elt->getAttributeNodeNS('http://www.w3.org/2000/svg', 'id');
$attr->value = '&lt;&amp;';
print "$attr->value\n";
print $doc->saveXML($elt) . "\n";
?>
--EXPECTF--
<elt a="&amp;"/>
&
<elt a="&amp;"/>
<elt a=""/>

Warning: main(): unterminated entity reference                 in %s on line %d

<elt a=""/>
&
<elt a="&amp;"/>
<elt xmlns:svg="http://www.w3.org/2000/svg" svg:id="&amp;amp;"/>
<&
<elt xmlns:svg="http://www.w3.org/2000/svg" svg:id="&lt;&amp;"/>
