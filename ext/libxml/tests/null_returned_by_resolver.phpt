--TEST--
null returned by resolver function
--EXTENSIONS--
libxml
dom
--FILE--
<?php

libxml_set_external_entity_loader(function ($public_id, $system_id, $context) {
    var_dump($public_id, $system_id, $context);
    return null;
});

$doc = new DOMDocument();
$doc->loadHTMLFile("foobar");

?>
--EXPECTF--
NULL
string(6) "foobar"
array(4) {
  ["directory"]=>
  NULL
  ["intSubName"]=>
  NULL
  ["extSubURI"]=>
  NULL
  ["extSubSystem"]=>
  NULL
}

Warning: DOMDocument::loadHTMLFile(): Failed to load external entity because the resolver function returned null in %s on line %d
