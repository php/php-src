--TEST--
Node list cache should not break on DOMElement::getElementsByTagName() without document
--EXTENSIONS--
dom
--FILE--
<?php

$element = new DOMElement("b");
foreach ($element->getElementsByTagName("b") as $x) {
    var_dump($x);
}

?>
Done
--EXPECT--
Done
