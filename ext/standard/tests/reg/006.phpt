--TEST--
Test ereg_replace of start-of-line
--FILE--
<?php $a="This is a nice and simple string";
  echo ereg_replace("^This","That",$a);
?>
--EXPECT--
That is a nice and simple string
