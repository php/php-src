--TEST--
DOMComment::__construct() with more arguments than acceptable.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--EXTENSIONS--
dom
--FILE--
<?php
try {
    $comment = new DOMComment("comment1", "comment2");
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ArgumentCountError: DOMComment::__construct() expects at most 1 argument, 2 given
