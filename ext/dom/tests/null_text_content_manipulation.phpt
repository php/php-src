--TEST--
NULL text content manipulation
--EXTENSIONS--
dom
--FILE--
<?php
$text = new DOMText();
try {
    var_dump($text->substringData(1, 0));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($text->insertData(1, ""));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($text->deleteData(1, 1));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($text->replaceData(1, 1, ""));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Index Size Error
Index Size Error
Index Size Error
Index Size Error
