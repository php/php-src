--TEST--
Testing DOMDocumentFragment::appendXML and DOMDocumentFragment::hasChildNodes
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument();

$fragment = $doc->createDocumentFragment();
if ($fragment->hasChildNodes()) {
  echo "has child nodes\n";
} else {
  echo "has no child nodes\n";
}
$fragment->appendXML('<foo>bar</foo>');
if ($fragment->hasChildNodes()) {
  echo "has child nodes\n";
} else {
  echo "has no child nodes\n";
}
--EXPECT--
has no child nodes
has child nodes
