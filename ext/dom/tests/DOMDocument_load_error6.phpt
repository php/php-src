--TEST--
Test DOMDocument::load() with invalid paths
--EXTENSIONS--
dom
--FILE--
<?php
// create dom document
$dom = new DOMDocument();
try {
    $dom->load("");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $dom->load("/path/with/\0/byte");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

// Path is too long
var_dump($dom->load(str_repeat(" ", PHP_MAXPATHLEN + 1)));
?>
--EXPECT--
DOMDocument::load(): Argument #1 ($filename) must not be empty
Path to document must not contain any null bytes
bool(false)
