--TEST--
Test to ensure that calling DOMXPath::__construct passing an invalid parameter fails
--CREDITS--
Vinicius Dias carlosv775@gmail.com
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
try {
    new DOMXpath(null);
} catch (\TypeError $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
DOMXPath::__construct() expects parameter 1 to be DOMDocument, null given
