--TEST--
substringData() negative arguments
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$comment = $dom->createComment("foobarbaz");
var_dump($comment->substringData(0, -1));
echo $dom->saveHtml($comment), "\n";
var_dump($comment->substringData(2, -2));
echo $dom->saveHtml($comment), "\n";
try {
    var_dump($comment->substringData(-2, 2));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHtml($comment), "\n";

?>
--EXPECT--
string(9) "foobarbaz"
<!--foobarbaz-->
string(7) "obarbaz"
<!--foobarbaz-->
Index Size Error
<!--foobarbaz-->
