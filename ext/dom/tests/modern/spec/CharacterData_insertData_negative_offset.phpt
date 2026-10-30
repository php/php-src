--TEST--
insertData() negative offset
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$comment = $dom->createComment("foobarbaz");
try {
    $comment->insertData(-1, "A");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHtml($comment), "\n";
$comment->insertData(1, "A");
echo $dom->saveHtml($comment), "\n";

?>
--EXPECT--
Index Size Error
<!--foobarbaz-->
<!--fAoobarbaz-->
