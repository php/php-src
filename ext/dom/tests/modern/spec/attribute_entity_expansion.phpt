--TEST--
Attribute entity expansion
--EXTENSIONS--
dom
--FILE--
<?php
$doc = Dom\HTMLDocument::createEmpty();
$elt = $doc->createElement('elt');
$doc->appendChild($elt);
$elt->setAttribute('a','&');
print $doc->saveHtml($elt) . "\n";

$attr = $elt->getAttributeNode('a');
$attr->value = '&amp;';
print "$attr->value\n";
print $doc->saveHtml($elt) . "\n";

$attr->removeChild($attr->firstChild);
print $doc->saveHtml($elt) . "\n";

$attr->nodeValue = '&';
print "$attr->nodeValue\n";
print $doc->saveHtml($elt) . "\n";

$attr->nodeValue = '&amp;';
print "$attr->nodeValue\n";
print $doc->saveHtml($elt) . "\n";

$elt->removeAttributeNode($attr);
$elt->setAttributeNS('http://www.w3.org/2000/svg', 'svg:id','&amp;');
print $doc->saveHtml($elt) . "\n";

$attr = $elt->getAttributeNodeNS('http://www.w3.org/2000/svg', 'id');
$attr->value = '&lt;&amp;';
print "$attr->value\n";
print $doc->saveHtml($elt) . "\n";
?>
--EXPECT--
<elt a="&amp;"></elt>
&amp;
<elt a="&amp;amp;"></elt>
<elt a=""></elt>
&
<elt a="&amp;"></elt>
&amp;
<elt a="&amp;amp;"></elt>
<elt svg:id="&amp;amp;"></elt>
&lt;&amp;
<elt svg:id="&amp;lt;&amp;amp;"></elt>
