--TEST--
getopt
--ARGS--
-v -h -d test -m 1234 -t -j
--SKIPIF--
<?php
	if (!ini_get('register_argc_argv')) {
		die("skip this test needs register_argc_argv to be enabled");
	}
?>
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
