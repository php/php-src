--TEST--
xmldoc()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("domxml_test.inc");

$dom = xmldoc($xmlstr);
if(!$dom) {
  die('Error while parsing the document');
}
var_dump($dom);
?>
--EXPECTF--
object(domdocument)(9) {
  ["name"]=>
  string(9) "#document"
  ["url"]=>
  string(0) ""
  ["version"]=>
  string(3) "1.0"
  ["standalone"]=>
  int(1)
  ["type"]=>
  int(9)
  ["compression"]=>
  int(-1)
  ["charset"]=>
  int(1)
  [0]=>
  int(5)
  [1]=>
  int(%d)
}