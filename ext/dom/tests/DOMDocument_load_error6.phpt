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
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $dom->load("/path/with/\0/byte");
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

// Path is too long
var_dump($dom->load(str_repeat(" ", PHP_MAXPATHLEN + 1)));
?>
--EXPECT--
ValueError: DOMDocument::load(): Argument #1 ($filename) must not be empty
ValueError: DOMDocument::load(): Argument #1 ($filename) must not contain any null bytes
bool(false)
