--TEST--
mb_ereg_replace() compatibility test 13 (counterpart: ext/standard/tests/reg/016.phpt)
--POST--
--GET--
--FILE--
<?php echo mb_ereg_replace('\?',"abc","?123?")?>
--EXPECT--
abc123abc
