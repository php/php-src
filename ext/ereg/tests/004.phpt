--TEST--
simple ereg test
--FILE--
<?php $a="This is a nice and simple string";
  if (ereg(".*nice and simple.*",$a)) {
    echo "ok\n";
  }
  if (!ereg(".*doesn't exist.*",$a)) {
    echo "ok\n";
  }
?>
--EXPECTF--
Deprecated: Function ereg() is deprecated in %s on line %d
ok

Deprecated: Function ereg() is deprecated in %s on line %d
ok
