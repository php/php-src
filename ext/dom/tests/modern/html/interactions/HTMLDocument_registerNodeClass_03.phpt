--TEST--
Dom\HTMLDocument::registerNodeClass 03
--EXTENSIONS--
dom
--FILE--
<?php

class Custom extends Dom\HTMLElement {
    public int $test = 1;

    public function reverseTagName(): string {
        var_dump($this->test);
        return strrev($this->tagName);
    }
}

$dom = Dom\HTMLDocument::createFromString("<div>foo</div>", LIBXML_NOERROR);
$dom->registerNodeClass("Dom\\HTMLElement", "Custom");

var_dump($dom->getElementsByTagName('div')[0]->reverseTagName());

?>
--EXPECT--
int(1)
string(3) "VID"
