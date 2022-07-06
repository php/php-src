--TEST--
xmlwriter_set_indent_string passing xmlwriter resource
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php
$temp_filename = __DIR__."/xmlwriter_set_indent_string.tmp";
    $fp = fopen($temp_filename, "w");
    fwrite ($fp, "Hi");
    fclose($fp);
$resource = xmlwriter_open_uri($temp_filename);
var_dump(xmlwriter_set_indent_string($resource, '  '));
?>
--CLEAN--
<?php
$temp_filename = __DIR__."/xmlwriter_set_indent_string.tmp";
unlink($temp_filename);
?>
--CREDITS--
Koen Kuipers koenk82@gmail.com
Theo van der Zee
#Test Fest Utrecht 09-05-2009
--EXPECT--
bool(true)
