--TEST--
Bug #47229 (preg_quote() doesn't escape -)
--FILE--
<?php

var_dump(preg_quote('-oh really?'));

// make sure there's no regression in matching
preg_match('/[a\-c]+/', 'a---b', $m);
var_dump($m);

preg_match('/[a\-c]+/', 'a\-', $m);
var_dump($m);

preg_match("/a\-{2,}/", 'a----a', $m);
var_dump($m);

preg_match("/a\-{1,}/", 'a\----a', $m);
var_dump($m);

?>
--EXPECTF--
string(13) "\-oh really\?"
array(1) {
  [0]=>
  string(4) "a---"
}
array(1) {
  [0]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(5) "a----"
}
array(0) {
}
