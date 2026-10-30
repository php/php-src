<?php

function test_helper(DOM\ParentNode $dom, string $selector, bool $only_name = false)
{
    echo "--- Selector: $selector ---\n";

    $all = $dom->querySelectorAll($selector);
    $single = $dom->querySelector($selector);

    if ((count($all) === 0 && $single !== null) || ($all[0] !== $single)) {
        throw new Exception('Mismatch in querySelector and querySelectorAll');
    }

    $list = [];
    foreach ($all as $node) {
        $list[] = $node;

        if ($only_name) {
            echo $node->nodeName, "\n";
            continue;
        }

        echo $dom->saveXML($node), "\n";
    }

    // If the element is in the list, then it must match, otherwise it must not
    // This loops over all the elements in the document and checks them
    foreach ($dom->querySelectorAll('*') as $node) {
        if (in_array($node, $list, true) !== $node->matches($selector)) {
            var_dump($node, $selector, in_array($node, $list, true), $node->matches($selector));
            echo $dom->saveXML($node), "\n";
            throw new Exception('Bug in Element::matches()');
        }
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
