--TEST--
Bug #26528 (HTML entities are not being decoded)
--SKIPIF--
<?php 
require_once("skipif.inc");
?>
--FILE--
<?php
	$sample = "<?xml version=\"1.0\"?><test attr=\"angle&lt;bracket\"/>";
	$parser = xml_parser_create();
	$res = xml_parse_into_struct($parser,$sample,$vals,$index);
	xml_parser_free($parser);
	var_dump($vals);
?>
--EXPECT--
array(1) {
  [0]=>
  array(4) {
    [u"tag"]=>
    unicode(4) "TEST"
    [u"type"]=>
    unicode(8) "complete"
    [u"level"]=>
    int(1)
    [u"attributes"]=>
    array(1) {
      [u"ATTR"]=>
      unicode(13) "angle<bracket"
    }
  }
}
