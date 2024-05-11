--TEST--
Element::toggleAttribute() xmlns spec compliance
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$container = $dom->appendChild($dom->createElement('container'));

$container->toggleAttribute('hidden');
$container->toggleAttribute('xmlns:foo');
$container->toggleAttribute('xmlns:bar');
$container->toggleAttribute('xmlns');

echo $dom->saveHtml(), "\n";

$container->toggleAttribute('hidden');
$container->toggleAttribute('xmlns:foo');
$container->toggleAttribute('xmlns:bar');
$container->toggleAttribute('xmlns');

echo $dom->saveHtml(), "\n";

?>
--EXPECT--
<container hidden="" xmlns:foo="" xmlns:bar="" xmlns=""></container>
<container></container>
