--TEST--
getopt
--ARGS--
-v -h -d test -m 1234 -t -j
--INI--
register_argc_argv=On
variables_order=GPS
--FILE--
<?php
    var_dump(getopt("d:m:j:vht"));
?>
--EXPECT--
array(5) {
  ["v"]=>
  bool(false)
  ["h"]=>
  bool(false)
  ["d"]=>
  string(4) "test"
  ["m"]=>
  string(4) "1234"
  ["t"]=>
  bool(false)
}
