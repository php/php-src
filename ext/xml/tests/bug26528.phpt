--TEST--
Bug #26528 (HTML entities are not being decoded)
--EXTENSIONS--
xml
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
    ["tag"]=>
    string(4) "TEST"
    ["type"]=>
    string(8) "complete"
    ["level"]=>
    int(1)
    ["attributes"]=>
    array(1) {
      ["ATTR"]=>
      string(13) "angle<bracket"
    }
  }
}
