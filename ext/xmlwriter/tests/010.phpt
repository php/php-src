--TEST--
xmlwriter_start/end_attribute()
--SKIPIF--
<?php
if (!extension_loaded("xmlwriter")) die("skip");
?>
--FILE--
<?php

$file = __DIR__.'/010.tmp';

$xw = xmlwriter_open_uri($file);

var_dump(xmlwriter_start_element($xw, "tag"));
var_dump(xmlwriter_start_attribute($xw, "attr"));
var_dump(xmlwriter_end_attribute($xw));

try {
    xmlwriter_start_attribute($xw, "-1");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

var_dump(xmlwriter_end_attribute($xw));

try {
    xmlwriter_start_attribute($xw, "\"");
} catch (ValueError $e) {
     echo $e->getMessage(), "\n";
}

var_dump(xmlwriter_end_attribute($xw));
var_dump(xmlwriter_end_element($xw));

// Force to write and empty the buffer
xmlwriter_flush($xw, empty: true);

unset($xw);

var_dump(file_get_contents($file));

@unlink($file);

echo "Done\n";
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
xmlwriter_start_attribute(): Argument #2 ($name) must be a valid attribute name, "-1" given
bool(false)
xmlwriter_start_attribute(): Argument #2 ($name) must be a valid attribute name, """ given
bool(false)
bool(true)
string(14) "<tag attr=""/>"
Done
