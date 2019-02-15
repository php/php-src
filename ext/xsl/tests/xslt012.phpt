--TEST--
Test 12: Using Associative Array of Parameters
--SKIPIF--
<?php require_once dirname(__FILE__) .'/skipif.inc'; ?>
--FILE--
<?php
echo "Test 12: Using Associative Array of Parameters";

$dom = new domDocument;
$dom->load(dirname(__FILE__)."/xslt.xml");
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

$xsl = new domDocument;
$xsl->load(dirname(__FILE__)."/xslt012.xsl");
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

$parameters = Array(
					'foo' => 'barbar',
					'foo1' => 'test',
					);

$proc->setParameter( "", $parameters);

print "\n";
print $proc->transformToXml($dom);
print "\n";
--EXPECT--
Test 12: Using Associative Array of Parameters
<?xml version="1.0" encoding="iso-8859-1"?>
<html><body>barbar
test
a1 b1 c1 <br/> 
a2 c2 <br/> 
ä3 b3 c3 <br/> 
</body></html>
