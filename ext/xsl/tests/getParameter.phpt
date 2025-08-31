--TEST--
XSLTProcessor::getParameter()
--EXTENSIONS--
xsl
--FILE--
<?php

$xslt = new XSLTProcessor;

// Interned string test
$xslt->setParameter('', 'foo', 'bar');
var_dump($xslt->getParameter("", "foo"));
$xslt->removeParameter('', 'foo');
var_dump($xslt->getParameter("", "foo"));

// Non-interned string test
$xslt->setParameter('', 'foo', str_repeat('"abcd"', mt_rand(2, 2) /* defeat SCCP */));
var_dump($xslt->getParameter("", "foo"));
$xslt->removeParameter('', 'foo');
var_dump($xslt->getParameter("", "foo"));

?>
--EXPECT--
string(3) "bar"
bool(false)
string(12) ""abcd""abcd""
bool(false)
