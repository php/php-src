<?php
function dumpAttr(Dom\Attr $attr) {
    echo "Attr: {$attr->name}\n";
    var_dump($attr->prefix);
    var_dump($attr->nodeName);
    var_dump($attr->namespaceURI);
}

function dumpAttrs(Dom\Element $element) {
    foreach ($element->attributes as $attr) {
        dumpAttr($attr);
    }
}
