--TEST--
Bug #20528 (preg_split() drops characters (re-opens Bug #15413)) 
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
--UEXPECT--
array(7) {
  [0]=>
  unicode(2) "(#"
  [1]=>
  unicode(2) "11"
  [2]=>
  unicode(1) "/"
  [3]=>
  unicode(2) "19"
  [4]=>
  unicode(1) "/"
  [5]=>
  unicode(4) "2002"
  [6]=>
  unicode(2) "#)"
}
