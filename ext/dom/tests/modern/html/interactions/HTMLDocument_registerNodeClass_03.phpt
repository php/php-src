--TEST--
DOM\HTMLDocument::registerNodeClass 03
--EXTENSIONS--
dom
--FILE--
<?php

class Custom extends DOM\Element {
    public int $test = 1;

    public function reverseTagName(): string {
        var_dump($this->test);
        return strrev($this->tagName);
    }
}

$dom = DOM\HTMLDocument::createFromString("<div>foo</div>", LIBXML_NOERROR);
$dom->registerNodeClass("DOM\\Element", "Custom");

var_dump($dom->getElementsByTagName('div')[0]->reverseTagName());

?>
--EXPECT--
int(1)
string(3) "VID"
