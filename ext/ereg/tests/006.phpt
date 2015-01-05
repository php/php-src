--TEST--
Test ereg_replace of start-of-line
--FILE--
<?php $a="This is a nice and simple string";
  echo ereg_replace("^This","That",$a);
?>
--EXPECTF--
Deprecated: Function ereg_replace() is deprecated in %s on line %d
That is a nice and simple string
