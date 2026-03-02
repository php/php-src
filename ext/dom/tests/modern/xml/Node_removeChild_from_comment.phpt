--TEST--
Removing a child from a comment should result in NOT_FOUND_ERR
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root><!-- comment --><child/></root>');
$comment = $dom->documentElement->firstChild;
$child = $comment->nextSibling;

try {
    $comment->removeChild($child);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Not Found Error
