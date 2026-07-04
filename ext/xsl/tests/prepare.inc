<?php
$dom = new DOMDocument;
$dom->load(dirname(__FILE__)."/xslt.xml");
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}
$xsl = new DOMDocument;
$xsl->load(dirname(__FILE__)."/xslt.xsl");
if(!$xsl) {
  echo "Error while parsing the document\n";
  exit;
}
$proc = new XSLTProcessor;
if(!$proc) {
  echo "Error while making xsltprocessor object\n";
  exit;
}

?>
