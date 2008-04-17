--TEST--
Bug #44648 (Attribute names not checked for wellformedness)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML('<root/>');

$root = $doc->documentElement;

try {
  $attr = new DOMAttr('@acb', '123');
  $root->setAttributeNode($attr);
} catch (DOMException $e) {
  echo $e->getMessage()."\n";
}

try {
  $root->setAttribute('@def', '456');
} catch (DOMException $e) {
  echo $e->getMessage()."\n";
}

try {
  $root->setAttributeNS(NULL, '@ghi', '789');
} catch (DOMException $e) {
  echo $e->getMessage()."\n";
}

try {
  $root->setAttributeNS('urn::test', 'a:g@hi', '789');
} catch (DOMException $e) {
  echo $e->getMessage()."\n";
}

echo $doc->saveXML($root);
?>
--EXPECT--
Invalid Character Error
Invalid Character Error
Invalid Character Error
Namespace Error
<root/>