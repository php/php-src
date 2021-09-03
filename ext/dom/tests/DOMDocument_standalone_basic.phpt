--TEST--
Tests DOMDocument::standalone get, set, and functionality
--CREDITS--
Chris Snyder <chsnyder@gmail.com>
# TestFest 2009 NYPHP
--EXTENSIONS--
dom
--FILE--
<?php
// create dom document
$dom = new DOMDocument;
$xml = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE s1 PUBLIC "http://www.ibm.com/example.dtd" "example.dtd">
<s1>foo</s1>';
$dom->loadXML($xml);
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}
echo "Standalone DOMDocument created\n";

$test = $dom->standalone;
echo "Read initial standalone:\n";
var_dump( $test );

$dom->standalone = FALSE;
$test = $dom->standalone;
echo "Set standalone to FALSE, reading again:\n";
var_dump( $test );

$test = $dom->saveXML();
echo "Document is no longer standalone\n";
var_dump( $test );

echo "Done";
?>
--EXPECT--
Standalone DOMDocument created
Read initial standalone:
bool(true)
Set standalone to FALSE, reading again:
bool(false)
Document is no longer standalone
string(136) "<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE s1 PUBLIC "http://www.ibm.com/example.dtd" "example.dtd">
<s1>foo</s1>
"
Done
