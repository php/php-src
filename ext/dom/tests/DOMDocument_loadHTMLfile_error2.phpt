--TEST--
Test DOMDocument::loadHTMLFile when an empty string is passed
--DESCRIPTION--
Verifies that an error message is showed if an empty string is passed as argument
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
try {
    $result = $doc->loadHTMLFile("");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$doc = new DOMDocument();
try {
    $result = $doc->loadHTMLFile("text.html\0something");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: DOMDocument::loadHTMLFile(): Argument #1 ($filename) must not be empty
ValueError: DOMDocument::loadHTMLFile(): Argument #1 ($filename) must not contain any null bytes
