--TEST--
Test DOMDocument::loadHTMLFile when an empty string is passed
--DESCRIPTION--
Verifies that an error message is showed if an empty string is passed as argument
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--INI--
assert.bail=true
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument();
try {
    $result = $doc->loadHTMLFile("");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

$doc = new DOMDocument();
try {
    $result = $doc->loadHTMLFile("text.html\0something");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
DOMDocument::loadHTMLFile(): Argument #1 ($filename) must not be empty
DOMDocument::loadHTMLFile(): Argument #1 ($filename) must not contain any null bytes
