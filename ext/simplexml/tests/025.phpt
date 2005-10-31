--TEST--
SimpleXML: getting namespaces
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$xml =<<<EOF
<?xml version='1.0'?>
<xhtml:html xmlns:html='http://www.w3.org/1999/xhtml' xmlns:xhtml='http://www.w3.org/TR/REC-html40'>
<xhtml:head><xhtml:title>bla</xhtml:title></xhtml:head>
<xhtml:body html:title="b">
<html:h1>bla</html:h1>
<foo:bar xmlns:foo='foobar' xmlns:baz='foobarbaz'/>
</xhtml:body>
</xhtml:html>
EOF;

$sxe = simplexml_load_string($xml);

var_dump($sxe->getNamespaces());
var_dump($sxe->getNamespaces(true));
var_dump($sxe->getDocNamespaces());
var_dump($sxe->getDocNamespaces(true));

?>
===DONE===
--EXPECTF--
array(1) {
  ["xhtml"]=>
  string(31) "http://www.w3.org/TR/REC-html40"
}
array(3) {
  ["xhtml"]=>
  string(31) "http://www.w3.org/TR/REC-html40"
  ["html"]=>
  string(28) "http://www.w3.org/1999/xhtml"
  ["foo"]=>
  string(6) "foobar"
}
array(2) {
  ["html"]=>
  string(28) "http://www.w3.org/1999/xhtml"
  ["xhtml"]=>
  string(31) "http://www.w3.org/TR/REC-html40"
}
array(4) {
  ["html"]=>
  string(28) "http://www.w3.org/1999/xhtml"
  ["xhtml"]=>
  string(31) "http://www.w3.org/TR/REC-html40"
  ["foo"]=>
  string(6) "foobar"
  ["baz"]=>
  string(9) "foobarbaz"
}
===DONE===
--UEXPECTF--
array(1) {
  [u"xhtml"]=>
  string(31) "http://www.w3.org/TR/REC-html40"
}
array(3) {
  [u"xhtml"]=>
  string(31) "http://www.w3.org/TR/REC-html40"
  [u"html"]=>
  string(28) "http://www.w3.org/1999/xhtml"
  [u"foo"]=>
  string(6) "foobar"
}
array(2) {
  [u"html"]=>
  string(28) "http://www.w3.org/1999/xhtml"
  [u"xhtml"]=>
  string(31) "http://www.w3.org/TR/REC-html40"
}
array(4) {
  [u"html"]=>
  string(28) "http://www.w3.org/1999/xhtml"
  [u"xhtml"]=>
  string(31) "http://www.w3.org/TR/REC-html40"
  [u"foo"]=>
  string(6) "foobar"
  [u"baz"]=>
  string(9) "foobarbaz"
}
===DONE===
