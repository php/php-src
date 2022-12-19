--TEST--
DOMAttr entity expansion
--FILE--
<?php
$doc = new DOMDocument;
$elt = $doc->createElement('elt');
$doc->appendChild($elt);
$elt->setAttribute('a','&');
print $doc->saveXML($elt) . "\n";

$attr = $elt->getAttributeNode('a');
$attr->value = '&amp;';
print $doc->saveXML($elt) . "\n";

$attr->removeChild($attr->firstChild);
print $doc->saveXML($elt) . "\n";

$elt->setAttributeNS('http://www.w3.org/2000/svg', 'svg:id','&amp;');
print $doc->saveXML($elt) . "\n";

$attr = $elt->getAttributeNodeNS('http://www.w3.org/2000/svg', 'id');
$attr->value = '&lt;&amp;';
print $doc->saveXML($elt) . "\n";

--EXPECT--
<elt a="&amp;"/>
<elt a="&amp;amp;"/>
<elt a=""/>
<elt xmlns:svg="http://www.w3.org/2000/svg" a="" svg:id="&amp;amp;"/>
<elt xmlns:svg="http://www.w3.org/2000/svg" a="" svg:id="&amp;lt;&amp;amp;"/>
