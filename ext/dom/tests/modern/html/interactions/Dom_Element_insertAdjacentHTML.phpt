--TEST--
Dom\Element::insertAdjacentHTML() with HTML nodes
--EXTENSIONS--
dom
--FILE--
<?php

const POSITIONS = [
    Dom\AdjacentPosition::BeforeBegin,
    Dom\AdjacentPosition::AfterBegin,
    Dom\AdjacentPosition::BeforeEnd,
    Dom\AdjacentPosition::AfterEnd,
];

function test(string $html) {
    echo "=== HTML ($html) ===\n";

    foreach (POSITIONS as $position) {
        echo "--- Position ", $position->name, " ---\n";

        $dom = Dom\HTMLDocument::createFromString("<div></div>", LIBXML_NOERROR);
        $div = $dom->body->firstChild;
        $div->append("Sample text");

        $div->insertAdjacentHTML($position, $html);

        echo $dom->saveXML(), "\n";
        echo $dom->saveHTML(), "\n";
        var_dump($div->childNodes->length);
        var_dump($dom->body->childNodes->length);
    }
}

test("<p>foo</p><p>bar</p>");
test("text");
test("");

?>
--EXPECT--
=== HTML (<p>foo</p><p>bar</p>) ===
--- Position BeforeBegin ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><p>foo</p><p>bar</p><div>Sample text</div></body></html>
<html><head></head><body><p>foo</p><p>bar</p><div>Sample text</div></body></html>
int(1)
int(3)
--- Position AfterBegin ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><div><p>foo</p><p>bar</p>Sample text</div></body></html>
<html><head></head><body><div><p>foo</p><p>bar</p>Sample text</div></body></html>
int(3)
int(1)
--- Position BeforeEnd ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><div>Sample text<p>foo</p><p>bar</p></div></body></html>
<html><head></head><body><div>Sample text<p>foo</p><p>bar</p></div></body></html>
int(3)
int(1)
--- Position AfterEnd ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><div>Sample text</div><p>foo</p><p>bar</p></body></html>
<html><head></head><body><div>Sample text</div><p>foo</p><p>bar</p></body></html>
int(1)
int(3)
=== HTML (text) ===
--- Position BeforeBegin ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body>text<div>Sample text</div></body></html>
<html><head></head><body>text<div>Sample text</div></body></html>
int(1)
int(2)
--- Position AfterBegin ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><div>textSample text</div></body></html>
<html><head></head><body><div>textSample text</div></body></html>
int(2)
int(1)
--- Position BeforeEnd ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><div>Sample texttext</div></body></html>
<html><head></head><body><div>Sample texttext</div></body></html>
int(2)
int(1)
--- Position AfterEnd ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><div>Sample text</div>text</body></html>
<html><head></head><body><div>Sample text</div>text</body></html>
int(1)
int(2)
=== HTML () ===
--- Position BeforeBegin ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><div>Sample text</div></body></html>
<html><head></head><body><div>Sample text</div></body></html>
int(1)
int(1)
--- Position AfterBegin ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><div>Sample text</div></body></html>
<html><head></head><body><div>Sample text</div></body></html>
int(1)
int(1)
--- Position BeforeEnd ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><div>Sample text</div></body></html>
<html><head></head><body><div>Sample text</div></body></html>
int(1)
int(1)
--- Position AfterEnd ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><div>Sample text</div></body></html>
<html><head></head><body><div>Sample text</div></body></html>
int(1)
int(1)
