--TEST--
Tests DOMDocument::resoleExternals get and set
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
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
 "http://www.w3.org/TR/html4/strict.dtd">
<h1>&quot;Foo&quot;</h1>';
$dom->loadXML($xml);
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}
echo "DOMDocument with external entities created\n";

$test = $dom->resolveExternals;
echo "Read initial resolveExternals:\n";
var_dump( $test );

$dom->resolveExternals = TRUE;
$test = $dom->resolveExternals;
echo "Set resolveExternals to TRUE, reading again:\n";
var_dump( $test );

/**
 * Don't bother testing the resolveExternals functionality here, it throws warnings on html dtd
 *
echo "Reloading xml with resolveExternals turned on\n";
$dom->loadXML($xml);
$test = $dom->saveXML();
var_dump( $test );
 */

echo "Done";
?>
--EXPECT--
DOMDocument with external entities created
Read initial resolveExternals:
bool(false)
Set resolveExternals to TRUE, reading again:
bool(true)
Done
