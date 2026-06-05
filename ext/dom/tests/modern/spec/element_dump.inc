<?php
function dumpElement(Dom\Element $element) {
    echo "tagName: ", var_dump($element->tagName);
    echo "nodeName: ", var_dump($element->nodeName);
    echo "textContent: ", var_dump($element->textContent);
    echo "prefix: ", var_dump($element->prefix);
    echo "namespaceURI: ", var_dump($element->namespaceURI);
    echo $element->ownerDocument->saveHtml($element), "\n\n";
}
