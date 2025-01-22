--TEST--
deleteData() negative in bounds length
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Modern behaviour ---\n";

$dom = Dom\HTMLDocument::createEmpty();
$comment = $dom->createComment("foobarbaz");
$comment->deleteData(3, -1);
echo $dom->saveHtml($comment), "\n";

echo "--- Legacy behaviour ---\n";

$dom = new DOMDocument;
$comment = $dom->createComment("foobarbaz");
try {
    $comment->deleteData(3, -1);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHtml($comment), "\n";

?>
--EXPECT--
--- Modern behaviour ---
<!--foo-->
--- Legacy behaviour ---
Index Size Error
<!--foobarbaz-->
