--TEST--
DOMComment::__construct() with more arguments than acceptable.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
try {
    $comment = new DOMComment("comment1", "comment2");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
DOMComment::__construct() expects at most 1 parameter, 2 given
