--TEST--
file(): FILE_TEXT, FILE_BINARY, and default behavior
--FILE--
<?php

$urls = array();
$urls[] = "data://text/plain,foo\nbar\n";
$urls[] = "data://text/plain,\nfoo\nbar\n";
$urls[] = "data://text/plain,foo\nbar";

foreach($urls as $url) {
	echo strtr($url, array("\r" => "\\r", "\n" => "\\n")) . "\n";
	var_dump(file($url, FILE_IGNORE_NEW_LINES));
	var_dump(file($url, FILE_IGNORE_NEW_LINES|FILE_TEXT));
	var_dump(file($url, FILE_IGNORE_NEW_LINES|FILE_BINARY));
}
?>
--EXPECTF--
data://text/plain,foo\nbar\n
array(2) {
  [0]=>
  unicode(3) "foo"
  [1]=>
  unicode(3) "bar"
}
array(2) {
  [0]=>
  unicode(3) "foo"
  [1]=>
  unicode(3) "bar"
}
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
data://text/plain,\nfoo\nbar\n
array(3) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(3) "foo"
  [2]=>
  unicode(3) "bar"
}
array(3) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(3) "foo"
  [2]=>
  unicode(3) "bar"
}
array(3) {
  [0]=>
  string(0) ""
  [1]=>
  string(3) "foo"
  [2]=>
  string(3) "bar"
}
data://text/plain,foo\nbar
array(2) {
  [0]=>
  unicode(3) "foo"
  [1]=>
  unicode(3) "bar"
}
array(2) {
  [0]=>
  unicode(3) "foo"
  [1]=>
  unicode(3) "bar"
}
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
