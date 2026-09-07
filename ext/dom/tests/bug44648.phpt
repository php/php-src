--TEST--
Bug #44648 (Attribute names not checked for well formedness)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML('<root/>');

$root = $doc->documentElement;

try {
  $attr = new DOMAttr('@acb', '123');
  $root->setAttributeNode($attr);
} catch (Throwable $e) {
  echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
  $root->setAttribute('@def', '456');
} catch (Throwable $e) {
  echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
  $root->setAttributeNS(NULL, '@ghi', '789');
} catch (Throwable $e) {
  echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
  $root->setAttributeNS('urn::test', 'a:g@hi', '789');
} catch (Throwable $e) {
  echo $e::class, ': ', $e->getMessage(), "\n";
}

echo $doc->saveXML($root);
?>
--EXPECT--
DOMException: Invalid Character Error
DOMException: Invalid Character Error
DOMException: Invalid Character Error
DOMException: Namespace Error
<root/>
