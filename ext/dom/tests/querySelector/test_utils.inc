<?php

function test_helper(DOM\ParentNode $dom, string $selector, bool $only_name = false)
{
    echo "--- Selector: $selector ---\n";

    $all = $dom->querySelectorAll($selector);
    $single = $dom->querySelector($selector);

    if ((count($all) === 0 && $single !== null) || ($all[0] !== $single)) {
        throw new Exception('Mismatch in querySelector and querySelectorAll');
    }

    foreach ($all as $node) {
        if ($only_name) {
            echo $node->nodeName, "\n";
            continue;
        }
        echo $dom->saveXML($node), "\n";
    }
}

function test_failure(DOM\ParentNode $dom, string $selector)
{
    echo "--- Selector: $selector ---\n";

    try {
        var_dump(count($dom->querySelectorAll($selector)));
    } catch (DOMException $e) {
        echo "Code ", $e->getCode(), " ", $e->getMessage(), "\n";
    }
}
