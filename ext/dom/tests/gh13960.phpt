--TEST--
GH-13960 (NULL bytes in XPath query)
--EXTENSIONS--
dom
--FILE--
<?php
$domd = new DOMDocument();
@$domd->loadHTML("<foo>tes\x00t</foo>");
$xp = new DOMXPath($domd);
try {
    $xp->query("//foo[contains(text(), " . $xp->quote("tes\x00t") . ")]");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
DOMXPath::quote(): Argument #1 ($str) must not contain any null bytes
