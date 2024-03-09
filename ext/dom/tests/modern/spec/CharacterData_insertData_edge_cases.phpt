--TEST--
insertData() edge cases
--EXTENSIONS--
dom
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only"); ?>
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$comment = $dom->createComment("foobarbaz");
try {
    var_dump($comment->insertData(100, "data"));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($comment->insertData(2**31+1, "data"));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHTML($comment), "\n";

?>
--EXPECT--
Index Size Error
Index Size Error
<!--foobarbaz-->
