--TEST--
libxml_use_internal_errors() memory leaks
--SKIPIF--
<?php if (!extension_loaded('simplexml')) die('skip'); ?>
--FILE--
<?php
var_dump(libxml_use_internal_errors(true));

$xmlstr = <<< XML
<?xml version='1.0' standalone='yes'?>
    <movies>
        <movie>
            <titles>PHP: Behind the Parser</title>
        </movie>
    </movies>
XML;

simplexml_load_string($xmlstr);

// test memleaks here
var_dump(libxml_use_internal_errors(false));

echo "Done\n";
?>
--EXPECT--
bool(false)
bool(true)
Done
