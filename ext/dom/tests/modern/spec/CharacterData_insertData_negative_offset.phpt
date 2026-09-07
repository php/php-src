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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
echo $dom->saveHtml($comment), "\n";
$comment->insertData(1, "A");
echo $dom->saveHtml($comment), "\n";

?>
--EXPECT--
DOMException: Index Size Error
<!--foobarbaz-->
<!--fAoobarbaz-->
