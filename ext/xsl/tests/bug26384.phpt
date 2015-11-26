--TEST--
Bug #26384 (domxslt->process causes segfault with xsl:key)
--SKIPIF--
<?php require_once dirname(__FILE__) .'/skipif.inc'; ?>
--FILE--
<?php
$dom = new domDocument;
$dom->load(dirname(__FILE__)."/area_name.xml");
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}
$xsl = new domDocument;
$xsl->load(dirname(__FILE__)."/area_list.xsl");
if(!$xsl) {
  echo "Error while parsing the document\n";
  exit;
}
$proc = new xsltprocessor;
if(!$proc) {
  echo "Error while making xsltprocessor object\n";
  exit;
}

$proc->importStylesheet($xsl);
print $proc->transformToXml($dom);

//this segfaulted before
print $dom->documentElement->firstChild->nextSibling->nodeName;

--EXPECT--
HERE
ROW
