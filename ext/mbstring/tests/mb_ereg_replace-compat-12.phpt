--TEST--
mb_ereg_replace() compatibility test 12 (counterpart: ext/standard/tests/reg/015.phpt)
--POST--
--GET--
--FILE--
<?php echo mb_ereg_replace("^","z","abc123")?>
--EXPECT--
zabc123
