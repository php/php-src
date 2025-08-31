--TEST--
replaceData() negative count (mod 32)
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (PHP_INT_SIZE === 4) die('skip not for 32-bit');
?>
--FILE--
<?php

echo "--- Modern behaviour ---\n";

$dom = Dom\HTMLDocument::createEmpty();
$comment = $dom->createComment("foobarbaz");
$comment->replaceData(0, -1, "A");
echo $dom->saveHtml($comment), "\n";
$comment = $dom->createComment("foobarbaz");
$comment->replaceData(2, -(2**32 - 2), "A");
echo $dom->saveHtml($comment), "\n";

echo "--- Legacy behaviour ---\n";

$dom = new DOMDocument;
$comment = $dom->createComment("foobarbaz");
try {
    $comment->replaceData(0, -1, "A");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHtml($comment), "\n";
try {
    $comment->replaceData(2, -(2**32 - 2), "A");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHtml($comment), "\n";

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
