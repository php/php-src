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
try {
    ini_get_all("");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    var_dump(ini_get_all("nosuchextension"));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
var_dump(ini_get_all("reflection"));
var_dump(ini_get_all("pcre"));
var_dump(ini_get_all("pcre", false));
var_dump(ini_get_all("reflection", false));
try {
    ini_get_all("", "");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECT--
string(5) "array"
Unable to find extension ''
Unable to find extension 'nosuchextension'
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
Unable to find extension ''
Done
