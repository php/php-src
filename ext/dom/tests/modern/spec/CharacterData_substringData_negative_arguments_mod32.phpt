--TEST--
substringData() negative arguments (mod 32)
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (PHP_INT_SIZE === 4) die('skip not for 32-bit');
?>
--FILE--
<?php

echo "--- Modern behaviour ---\n";

$dom = DOM\HTMLDocument::createEmpty();
$comment = $dom->createComment("foobarbaz");
var_dump($comment->substringData(0, -1));
echo $dom->saveHTML($comment), "\n";
var_dump($comment->substringData(2, -(2**32 - 2)));
echo $dom->saveHTML($comment), "\n";
var_dump($comment->substringData(-(2**32 - 2), 2));
echo $dom->saveHTML($comment), "\n";

echo "--- Legacy behaviour ---\n";

$dom = new DOMDocument;
$comment = $dom->createComment("foobarbaz");
try {
    var_dump($comment->substringData(0, -1));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHTML($comment), "\n";
try {
    var_dump($comment->substringData(2, -(2**32 - 2)));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHTML($comment), "\n";
try {
    var_dump($comment->substringData(-(2**32 - 2), 2));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveHTML($comment), "\n";

?>
--EXPECT--
--- Modern behaviour ---
string(9) "foobarbaz"
<!--foobarbaz-->
string(2) "ob"
<!--foobarbaz-->
string(2) "ob"
<!--foobarbaz-->
--- Legacy behaviour ---
Index Size Error
<!--foobarbaz-->
Index Size Error
<!--foobarbaz-->
Index Size Error
<!--foobarbaz-->
