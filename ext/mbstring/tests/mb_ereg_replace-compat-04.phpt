--TEST--
mb_ereg_replace() compatibility test 4 (counterpart: ext/standard/tests/reg/006.phpt)
--POST--
--GET--
--FILE--
<?php $a="This is a nice and simple string";
  echo mb_ereg_replace("^This","That",$a);
?>
--EXPECT--
That is a nice and simple string
