--TEST--
Bug #61858 DOMAttr debug info generates E_WARNING
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<example a="b">Test</example>');

$example = $doc->getElementsByTagName('example')->item(0);
$attr    = $example->getAttributeNode('a');

var_dump($attr);
print_r($attr);
?>
--EXPECTF--
object(DOMAttr)#%d (%d) {
%A
}
DOMAttr Object
(
%A
)
