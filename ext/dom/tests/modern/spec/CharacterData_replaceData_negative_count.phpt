--TEST--
replaceData() negative count
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$comment = $dom->createComment("foobarbaz");
$comment->replaceData(0, -1, "A");
echo $dom->saveHTML($comment), "\n";
$comment = $dom->createComment("foobarbaz");
$comment->replaceData(2, -2, "A");
echo $dom->saveHTML($comment), "\n";

?>
--EXPECT--
<!--A-->
<!--foA-->
