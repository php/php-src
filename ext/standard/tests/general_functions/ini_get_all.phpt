--TEST--
ini_get_all() tests
--INI--
pcre.backtrack_limit=100000
pcre.recursion_limit=100000
--SKIPIF--
<?php if (!extension_loaded("reflection")) die("skip"); ?>
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
array(2) {
  ["pcre.backtrack_limit"]=>
  array(3) {
    ["global_value"]=>
    string(6) "100000"
    ["local_value"]=>
    string(6) "100000"
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
array(2) {
  ["pcre.backtrack_limit"]=>
  string(6) "100000"
  ["pcre.recursion_limit"]=>
  string(6) "100000"
}
array(0) {
}

Warning: ini_get_all(): Unable to find extension '' in %sini_get_all.php on line %d
bool(false)
Done
--UEXPECTF--
unicode(5) "array"

Warning: ini_get_all(): Unable to find extension '' in %s on line %d
bool(false)

Warning: ini_get_all(): Unable to find extension 'nosuchextension' in %s on line %d
bool(false)
array(0) {
}
array(2) {
  [u"pcre.backtrack_limit"]=>
  array(3) {
    [u"global_value"]=>
    unicode(6) "100000"
    [u"local_value"]=>
    unicode(6) "100000"
    [u"access"]=>
    int(7)
  }
  [u"pcre.recursion_limit"]=>
  array(3) {
    [u"global_value"]=>
    unicode(6) "100000"
    [u"local_value"]=>
    unicode(6) "100000"
    [u"access"]=>
    int(7)
  }
}
array(2) {
  [u"pcre.backtrack_limit"]=>
  unicode(6) "100000"
  [u"pcre.recursion_limit"]=>
  unicode(6) "100000"
}
array(0) {
}

Warning: ini_get_all(): Unable to find extension '' in %sini_get_all.php on line %d
bool(false)
Done
