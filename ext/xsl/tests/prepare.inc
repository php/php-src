<?php
$dom = new domDocument;
$dom->load(dirname(__FILE__)."/xslt.xml");
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}
$xsl = new domDocument;
$xsl->load(dirname(__FILE__)."/xslt.xsl");
if(!$xsl) {
  echo "Error while parsing the document\n";
  exit;
}
$proc = new xsltprocessor;
if(!$proc) {
  echo "Error while making xsltprocessor object\n";
  exit;
}

?>
