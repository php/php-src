--TEST--
Bug #44034
--INI--
allow_url_fopen=1
--FILE--
<?php

$urls = array();
$urls[] = "data://text/plain,foo\r\nbar\r\n";
$urls[] = "data://text/plain,\r\nfoo\r\nbar\r\n";
$urls[] = "data://text/plain,foo\r\nbar";

foreach($urls as $url) {
    echo strtr($url, array("\r" => "\\r", "\n" => "\\n")) . "\n";
    var_dump(file($url, FILE_IGNORE_NEW_LINES));
}
?>
--EXPECT--
data://text/plain,foo\r\nbar\r\n
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
data://text/plain,\r\nfoo\r\nbar\r\n
array(3) {
  [0]=>
  string(0) ""
  [1]=>
  string(3) "foo"
  [2]=>
  string(3) "bar"
}
data://text/plain,foo\r\nbar
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
