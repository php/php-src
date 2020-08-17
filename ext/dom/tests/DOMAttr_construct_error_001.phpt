--TEST--
DOMAttr __construct() with no arguments.
--CREDITS--
Josh Sweeney <jsweeney@alt-invest.net>
# TestFest Atlanta 2009-05-14
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
try {
    $attr = new DOMAttr();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
DOMAttr::__construct(): At least 1 argument is expected, 0 given
