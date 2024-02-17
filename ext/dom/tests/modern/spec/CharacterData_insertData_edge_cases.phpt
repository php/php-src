--TEST--
insertData() edge cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$comment = $dom->createComment("foobarbaz");
try {
    var_dump($comment->insertData(100, "data"));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHTML($comment), "\n";

?>
--EXPECT--
Index Size Error
<!--foobarbaz-->
