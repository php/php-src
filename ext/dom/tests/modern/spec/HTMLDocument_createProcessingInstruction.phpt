--TEST--
Dom\HTMLDocument::createProcessingInstruction()
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createEmpty();
try {
    $dom->createProcessingInstruction("?>", "");
} catch (DOMException $e) {
    var_dump($e->getCode());
    echo $e->getMessage(), "\n";
}
try {
    $dom->createProcessingInstruction("?>", "?>");
} catch (DOMException $e) {
    var_dump($e->getCode());
    echo $e->getMessage(), "\n";
}
try {
    $dom->createProcessingInstruction("target", "?>");
} catch (DOMException $e) {
    var_dump($e->getCode());
    echo $e->getMessage(), "\n";
}
$dom->appendChild($dom->createProcessingInstruction("foo", ""));
$dom->appendChild($dom->createProcessingInstruction("foo", "bar"));
echo $dom->saveHtml();
?>
--EXPECT--
int(5)
Invalid Character Error
int(5)
Invalid Character Error
int(5)
Invalid character sequence "?>" in processing instruction
<?foo ><?foo bar>
