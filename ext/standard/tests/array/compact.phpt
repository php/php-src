--TEST--
compact()
--INI--
unicode.script_encoding=UTF-8
unicode.output_encoding=UTF-8
--FILE--
<?php

$çity  = "San Francisco";
$state = "CA";
$event = "SIGGRAPH";

$location_vars = array("c\u0327ity", "state");

$result = compact("event", $location_vars);
var_dump($result);
?>
--EXPECT--
array(3) {
  [u"event"]=>
  unicode(8) "SIGGRAPH"
  [u"çity"]=>
  unicode(13) "San Francisco"
  [u"state"]=>
  unicode(2) "CA"
}
