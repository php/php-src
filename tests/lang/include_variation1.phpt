--TEST--
include() a file from the current script directory
--FILE--
<?php
include("inc.inc");
?>
--EXPECT--
Included!
