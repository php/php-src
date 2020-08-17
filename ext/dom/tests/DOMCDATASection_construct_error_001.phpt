--TEST--
__construct() with no arguments.
--CREDITS--
Nic Rosental nicrosental@gmail.com
# TestFest Atlanta 2009-5-14
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
    try {
        $section = new DOMCDataSection();
    } catch (TypeError $e) {
        echo $e->getMessage();
    }
?>
--EXPECT--
DOMCdataSection::__construct(): Exactly 1 argument is expected, 0 given
