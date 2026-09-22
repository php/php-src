--TEST--
unpacking vs foreach in new DOM
--EXTENSIONS--
dom
--FILE--
<?php

$html = Dom\HTMLDocument::createFromString
(
    source: "<h1>Hi</h1><p>hi</p>",
    options: LIBXML_NOERROR,
);

foreach ($html->body->childNodes as $node) {
    echo $node->localName, "\n";
}

echo "---\n";

foreach ([...$html->body->childNodes] as $node) {
    echo $node->localName, "\n";
}

?>
--EXPECT--
h1
p
---
h1
p
