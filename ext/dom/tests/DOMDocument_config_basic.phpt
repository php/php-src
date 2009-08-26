--TEST--
Tests DOMDocument::config read
--CREDITS--
Chris Snyder <chsnyder@gmail.com>
# TestFest 2009 NYPHP
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
// create dom document
$dom = new DOMDocument;
echo "DOMDocument created\n";

$test = $dom->config;
echo "Read config:\n";
var_dump( $test );

// note -- will always be null as DOMConfiguration is not implemented in PHP

echo "Done\n";
?>
--EXPECT--
DOMDocument created
Read config:
NULL
Done
