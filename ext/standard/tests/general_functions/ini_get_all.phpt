--TEST--
ini_get_all() tests
--INI--
pcre.jit=1
pcre.backtrack_limit=1000000
pcre.recursion_limit=100000
--SKIPIF--
<?php if (!PCRE_JIT_SUPPORT) die("skip no pcre jit support"); ?>
--FILE--
<?php

var_dump(gettype(ini_get_all()));
var_dump(ini_get_all(""));
var_dump(ini_get_all("nosuchextension"));
var_dump(ini_get_all("reflection"));
var_dump(ini_get_all("pcre"));
var_dump(ini_get_all("pcre", false));
var_dump(ini_get_all("reflection", false));

var_dump(ini_get_all("", ""));

echo "Done\n";
?>
--EXPECTF--
string(5) "array"

Warning: ini_get_all(): Unable to find extension '' in %s on line %d
bool(false)

Warning: ini_get_all(): Unable to find extension 'nosuchextension' in %s on line %d
bool(false)
array(0) {
}
array(3) {
  ["pcre.backtrack_limit"]=>
  array(3) {
    ["global_value"]=>
    string(7) "1000000"
    ["local_value"]=>
    string(7) "1000000"
    ["access"]=>
    int(7)
  }
  ["pcre.jit"]=>
  array(3) {
    ["global_value"]=>
    string(1) "1"
    ["local_value"]=>
    string(1) "1"
    ["access"]=>
    int(7)
  }
  ["pcre.recursion_limit"]=>
  array(3) {
    ["global_value"]=>
    string(6) "100000"
    ["local_value"]=>
    string(6) "100000"
    ["access"]=>
    int(7)
  }
}
array(3) {
  ["pcre.backtrack_limit"]=>
  string(7) "1000000"
  ["pcre.jit"]=>
  string(1) "1"
  ["pcre.recursion_limit"]=>
  string(6) "100000"
}
array(0) {
}

Warning: ini_get_all(): Unable to find extension '' in %sini_get_all.php on line %d
bool(false)
Done
