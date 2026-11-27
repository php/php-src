--TEST--
Test Uri\QueryParams::sortByBytes() - success - Unicode order, surrogate pairs and prefixes
--FILE--
<?php

$params1 = (new Uri\QueryParams())
    ->append("\u{E000}", "private")
    ->append("\u{1F601}", "grin")
    ->append("\u{1F600}a", "smile-prefix")
    ->append("\u{10000}", "supplementary")
    ->append("\u{1F600}", "smile")
    ->append("\u{FFFF}", "last-bmp")
    ->append("\u{FEFF}", "bom")
    ->append("\u{E9}", "composed")
    ->append("e\u{301}", "decomposed")
    ->append("\u{D7FF}", "before-surrogates");
$params2 = $params1->sortByBytes();

var_dump($params2->toRfc3986String());

?>
--EXPECT--
string(204) "e%CC%81=decomposed&%C3%A9=composed&%ED%9F%BF=before-surrogates&%EE%80%80=private&%EF%BB%BF=bom&%EF%BF%BF=last-bmp&%F0%90%80%80=supplementary&%F0%9F%98%80=smile&%F0%9F%98%80a=smile-prefix&%F0%9F%98%81=grin"
