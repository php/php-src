--TEST--
Bug #26384 (domxslt->process causes segfault with xsl:key)
--EXTENSIONS--
xsl
--FILE--
<?php
$dom = new DOMDocument;
$dom->load(__DIR__."/area_name.xml");
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}
$xsl = new DOMDocument;
$xsl->load(__DIR__."/area_list.xsl");
if(!$xsl) {
  echo "Error while parsing the document\n";
  exit;
}
$proc = new XSLTProcessor;
if(!$proc) {
  echo "Error while making xsltprocessor object\n";
  exit;
}

$proc->importStylesheet($xsl);
print $proc->transformToXml($dom);

//this segfaulted before
print $dom->documentElement->firstChild->nextSibling->nodeName;
?>
--EXPECT--
HERE
ROW
