<?php
function dumpNodeList($nodeList) {
    echo "Node list length: ";
    var_dump($nodeList->length);
    foreach ($nodeList as $node) {
        echo "\t{$node->nodeName} {$node->textContent}\n";
    }
    echo "---\n";
}
