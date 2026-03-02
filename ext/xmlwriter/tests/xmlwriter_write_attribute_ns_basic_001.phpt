--TEST--
xmlwriter_write_attribute_ns basic function tests
--EXTENSIONS--
xmlwriter
--FILE--
<?php
$xw = xmlwriter_open_memory();
xmlwriter_set_indent($xw, TRUE);
xmlwriter_start_document($xw, NULL, "UTF-8");
xmlwriter_start_element($xw, 'root');
xmlwriter_write_attribute_ns($xw, 'prefix', 'id', 'http://www.php.net/uri', 'elem1');
xmlwriter_start_element($xw, 'elem1');
xmlwriter_write_attribute($xw, 'attr1', 'first');
xmlwriter_end_element($xw);
xmlwriter_full_end_element($xw);
xmlwriter_end_document($xw);

$output = xmlwriter_flush($xw, true);
print $output;
?>
--CREDITS--
Koen Kuipers koenk82@gmail.com
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root prefix:id="elem1" xmlns:prefix="http://www.php.net/uri">
 <elem1 attr1="first"/>
</root>
