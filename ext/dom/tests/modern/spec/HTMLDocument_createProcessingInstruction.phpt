--TEST--
Dom\HTMLDocument::createProcessingInstruction()
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createEmpty();
try {
    $dom->createProcessingInstruction("?>", "");
} catch (Throwable $e) {
    var_dump($e->getCode());
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $dom->createProcessingInstruction("?>", "?>");
} catch (Throwable $e) {
    var_dump($e->getCode());
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $dom->createProcessingInstruction("target", "?>");
} catch (Throwable $e) {
    var_dump($e->getCode());
    echo $e::class, ': ', $e->getMessage(), "\n";
}
$dom->appendChild($dom->createProcessingInstruction("foo", ""));
$dom->appendChild($dom->createProcessingInstruction("foo", "bar"));
echo $dom->saveHtml();
?>
--EXPECT--
int(5)
DOMException: Invalid Character Error
int(5)
DOMException: Invalid Character Error
int(5)
DOMException: Invalid character sequence "?>" in processing instruction
<?foo ><?foo bar>
