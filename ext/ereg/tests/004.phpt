--TEST--
simple ereg test
--SKIPIF--
<?php if (unicode_semantics()) die('skip unicode.semantics=on'); ?>        
--FILE--
<?php $a="This is a nice and simple string";
  if (ereg(".*nice and simple.*",$a)) {
    echo "ok\n";
  }
  if (!ereg(".*doesn't exist.*",$a)) {
    echo "ok\n";
  }
?>
--EXPECT--
ok
ok
