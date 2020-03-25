<?php
include(dirname(__FILE__) . '/domdocumentload_utilities.inc');

$doc = new DOMDocument();

$libxml_options = libxml_options_to_int(getenv('LOAD_OPTIONS'));
$result = $doc->loadXML(file_get_contents(dirname(__FILE__) . getenv('XML_FILE')),
    $libxml_options);

$expectedResult = (bool) getenv('EXPECTED_RESULT');
assert($result === $expectedResult);
?>
