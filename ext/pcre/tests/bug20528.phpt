--TEST--
preg_split() bug #20528
--SKIPIF--
<?php
	if (!extension_loaded("pcre")) {
		die("skip pcre extension is not available");
	}
?>
--FILE--
<?php
	$data = '(#11/19/2002#)';
	var_dump(preg_split('/\b/', $data));
?>
--EXPECT--
array(7) {
  [0]=>
  string(2) "(#"
  [1]=>
  string(2) "11"
  [2]=>
  string(1) "/"
  [3]=>
  string(2) "19"
  [4]=>
  string(1) "/"
  [5]=>
  string(4) "2002"
  [6]=>
  string(2) "#)"
}
