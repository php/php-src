--TEST--
insertData() negative offset (mod 32)
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
try {
    $comment->insertData(-1, "A");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHtml($comment), "\n";
$comment->insertData(-(2**32 - 1), "A");
echo $dom->saveHtml($comment), "\n";

echo "--- Legacy behaviour ---\n";

$dom = new DOMDocument;
$comment = $dom->createComment("foobarbaz");
try {
    $comment->insertData(-1, "A");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHtml($comment), "\n";
try {
    $comment->insertData(-(2**32 - 1), "A");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHtml($comment), "\n";

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
