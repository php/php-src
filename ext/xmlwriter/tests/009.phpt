--TEST--
XMLWriter: PI, Comment, CDATA
--SKIPIF--
<?php
if (!extension_loaded("xmlwriter")) die("skip");
if (!function_exists("xmlwriter_start_comment")) die("skip: libxml2 2.6.7+ required");
?>
--FILE--
<?php
/*
Libxml 2.6.24 and up adds a new line after a processing instruction (PI)
*/
$xw = xmlwriter_open_memory();
xmlwriter_set_indent($xw, TRUE);
xmlwriter_start_document($xw, NULL, "UTF-8");
xmlwriter_start_element($xw, 'root');
xmlwriter_write_attribute($xw, 'id', 'elem1');
xmlwriter_start_element($xw, 'elem1');
xmlwriter_write_attribute($xw, 'attr1', 'first');
xmlwriter_write_comment($xw, 'start PI');
xmlwriter_start_element($xw, 'pi');
xmlwriter_write_pi($xw, 'php', 'echo "hello world"; ');
xmlwriter_end_element($xw);
xmlwriter_start_element($xw, 'cdata');
xmlwriter_start_cdata($xw);
xmlwriter_text($xw, '<>&"');
xmlwriter_end_cdata($xw);
xmlwriter_end_element($xw);
xmlwriter_end_element($xw);
xmlwriter_end_element($xw);
xmlwriter_end_document($xw);
// Force to write and empty the buffer
$output = xmlwriter_flush($xw, true);
print $output;
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<root id="elem1">
 <elem1 attr1="first">
  <!--start PI-->
  <pi><?php echo "hello world"; ?>%w</pi>
  <cdata><![CDATA[<>&"]]></cdata>
 </elem1>
</root>
