<?php
include(dirname(__FILE__) . '/domdocumentload_utilities.php');

$doc = new DOMDocument();

$libxml_options = libxml_options_to_int(getenv('LOAD_OPTIONS'));

$result = $doc->load(dirname(__FILE__) . getenv('XML_FILE'), $libxml_options);

$expectedResult = (bool) getenv('EXPECTED_RESULT');
assert('$result === $expectedResult');

echo $doc->saveXML();
?>
