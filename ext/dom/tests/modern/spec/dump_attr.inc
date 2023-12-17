<?php
function dumpAttr(DOM\Attr $attr) {
    echo "Attr: {$attr->name}\n";
    var_dump($attr->prefix);
    var_dump($attr->nodeName);
    var_dump($attr->namespaceURI);
}
