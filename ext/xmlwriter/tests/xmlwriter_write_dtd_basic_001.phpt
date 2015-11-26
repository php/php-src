--TEST--
xmlwriter_write_dtd basic function tests
--SKIPIF--
<?php if (!extension_loaded("xmlwriter")) print "skip"; ?>
--FILE--
<?php

$xmlwriter = xmlwriter_open_memory();
var_dump(xmlwriter_write_dtd($xmlwriter, 'bla1', 'bla2', 'bla3', 'bla4'));
$output = xmlwriter_flush($xmlwriter, true);
print $output . PHP_EOL;

var_dump(xmlwriter_write_dtd($xmlwriter, '', '', ''));
$output = xmlwriter_flush($xmlwriter, true);
print $output;
?>
--CREDITS--
Koen Kuipers koenk82@gmail.com
--EXPECT--
bool(true)
<!DOCTYPE bla1 PUBLIC "bla2" "bla3" [bla4]>
bool(false)
