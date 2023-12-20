--TEST--
replaceData() negative count
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Modern behaviour ---\n";

$dom = DOM\HTMLDocument::createEmpty();
$comment = $dom->createComment("foobarbaz");
$comment->replaceData(0, -1, "A");
echo $dom->saveHTML($comment), "\n";
$comment = $dom->createComment("foobarbaz");
$comment->replaceData(2, -(2**32 - 2), "A");
echo $dom->saveHTML($comment), "\n";

echo "--- Legacy behaviour ---\n";

$dom = new DOMDocument;
$comment = $dom->createComment("foobarbaz");
try {
    $comment->replaceData(0, -1, "A");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHTML($comment), "\n";
try {
    $comment->replaceData(2, -(2**32 - 2), "A");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHTML($comment), "\n";

?>
--EXPECT--
--- Modern behaviour ---
<!--A-->
<!--foAarbaz-->
--- Legacy behaviour ---
Index Size Error
<!--foobarbaz-->
Index Size Error
<!--foobarbaz-->
