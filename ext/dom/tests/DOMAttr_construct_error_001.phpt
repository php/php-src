--TEST--
DOMAttr __construct() with no arguments.
--CREDITS--
Josh Sweeney <jsweeney@alt-invest.net>
# TestFest Atlanta 2009-05-14
--EXTENSIONS--
dom
--FILE--
<?php
try {
    $attr = new DOMAttr();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
DOMAttr::__construct() expects at least 1 argument, 0 given
