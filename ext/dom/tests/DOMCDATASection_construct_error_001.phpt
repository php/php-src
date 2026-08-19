--TEST--
__construct() with no arguments.
--CREDITS--
Nic Rosental nicrosental@gmail.com
# TestFest Atlanta 2009-5-14
--EXTENSIONS--
dom
--FILE--
<?php
    try {
        $section = new DOMCDataSection();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
?>
--EXPECT--
ArgumentCountError: DOMCdataSection::__construct() expects exactly 1 argument, 0 given
