--TEST--
substringData() edge cases
--EXTENSIONS--
dom
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only"); ?>
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$comment = $dom->createComment("foobarbaz");
try {
    var_dump($comment->substringData(0, 2**31+1));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($comment->substringData(2**31+1, 0));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($comment->substringData(100, 0));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHTML($comment), "\n";

?>
--EXPECT--
Index Size Error
Index Size Error
Index Size Error
<!--foobarbaz-->
