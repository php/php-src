--TEST--
Simple If condition test
--POST--
--GET--
--FILE--
<?php $a=1; if($a>0) { echo "Yes"; } ?>
--EXPECT--
Yes
