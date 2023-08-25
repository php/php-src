--TEST--
DOM\HTML5Document::registerNodeClass 01
--EXTENSIONS--
dom
--FILE--
<?php

class CustomDOMHTML5Document extends DOM\HTML5Document {
    public function test() {
        return $this->firstChild->textContent;
    }
}

$dom = new DOM\HTML5Document();
$dom->registerNodeClass("DOM\HTML5Document", "CustomDOMHTML5Document");
$dom->loadHTML("<p>foo</p>", LIBXML_NOERROR);

$element = $dom->documentElement;
unset($dom);

$dom = $element->ownerDocument;
var_dump($dom instanceof CustomDOMHTML5Document);
var_dump($dom->test());

?>
--EXPECT--
bool(true)
string(3) "foo"
