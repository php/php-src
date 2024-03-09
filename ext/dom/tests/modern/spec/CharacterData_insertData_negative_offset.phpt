--TEST--
insertData() negative offset
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- Modern behaviour ---\n";

$dom = DOM\HTMLDocument::createEmpty();
$comment = $dom->createComment("foobarbaz");
try {
    $comment->insertData(-1, "A");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHTML($comment), "\n";
$comment->insertData(-(2**32 - 1), "A");
echo $dom->saveHTML($comment), "\n";

echo "--- Legacy behaviour ---\n";

$dom = new DOMDocument;
$comment = $dom->createComment("foobarbaz");
try {
    $comment->insertData(-1, "A");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHTML($comment), "\n";
try {
    $comment->insertData(-(2**32 - 1), "A");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHTML($comment), "\n";

?>
--EXPECT--
--- Modern behaviour ---
Index Size Error
<!--foobarbaz-->
<!--fAoobarbaz-->
--- Legacy behaviour ---
Index Size Error
<!--foobarbaz-->
Index Size Error
<!--foobarbaz-->
