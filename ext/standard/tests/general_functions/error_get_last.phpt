--TEST--
error_get_last() tests
--FILE--
<?php

var_dump(error_get_last());
var_dump(error_get_last(true));
var_dump(error_get_last());

$a = $b;

var_dump(error_get_last());

echo "Done\n";
?>
--EXPECTF--
NULL

Warning: error_get_last() expects exactly 0 parameters, 1 given in %s on line %d
NULL
array(4) {
  [u"type"]=>
  int(2)
  [u"message"]=>
  unicode(54) "error_get_last() expects exactly 0 parameters, 1 given"
  [u"file"]=>
  unicode(%d) "%s"
  [u"line"]=>
  int(%d)
}

Notice: Undefined variable: b in %s on line %d
array(4) {
  [u"type"]=>
  int(8)
  [u"message"]=>
  unicode(21) "Undefined variable: b"
  [u"file"]=>
  unicode(%d) "%s"
  [u"line"]=>
  int(%d)
}
Done
