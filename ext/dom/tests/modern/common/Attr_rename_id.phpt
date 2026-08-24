--TEST--
Dom\Attr::rename() updates the id used by getElementById()
--EXTENSIONS--
dom
--FILE--
<?php

foreach ([
    Dom\XMLDocument::createFromString('<root><a/><b/></root>'),
    Dom\HTMLDocument::createFromString('<!DOCTYPE html><a></a><b></b>', LIBXML_NOERROR),
] as $dom) {
    echo "--- ", $dom::class, " ---\n";
    $a = $dom->getElementsByTagName("a")->item(0);
    $b = $dom->getElementsByTagName("b")->item(0);

    $a->setAttribute("id", "x");
    var_dump($dom->getElementById("x")?->localName);

    $a->getAttributeNode("id")->rename(null, "foo");
    var_dump($a->getAttribute("id"));
    var_dump($a->getAttribute("foo"));
    var_dump($dom->getElementById("x")?->localName);

    $b->setAttribute("foo", "y");
    $b->getAttributeNode("foo")->rename(null, "id");
    var_dump($dom->getElementById("y")?->localName);

    $b->getAttributeNode("id")->rename("urn:test", "id");
    var_dump($dom->getElementById("y")?->localName);

    $b->setAttribute("key", "k");
    $b->setIdAttribute("key", true);
    $b->getAttributeNode("key")->rename(null, "key");
    var_dump($dom->getElementById("k")?->localName);
}

?>
--EXPECT--
--- Dom\XMLDocument ---
string(1) "a"
NULL
string(1) "x"
NULL
string(1) "b"
NULL
string(1) "b"
--- Dom\HTMLDocument ---
string(1) "a"
NULL
string(1) "x"
NULL
string(1) "b"
NULL
string(1) "b"
