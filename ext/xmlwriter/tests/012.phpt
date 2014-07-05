--TEST--
XMLWriter: libxml2 XML Writer, full_end_element function
--CREDITS--
Mauricio Vieira <mauricio [at] @mauriciovieira [dot] net>
#testfest PHPSP on 2014-07-05
--SKIPIF--
<?php
if (!extension_loaded("xmlwriter")) die("skip");
if (LIBXML_VERSION < 20617) die("skip: libxml2 2.6.17+ required");
?>
--FILE--
<?php
/* $Id$ */

$xw = xmlwriter_open_memory();
xmlwriter_set_indent($xw, TRUE);
xmlwriter_set_indent_string($xw, '   ');
xmlwriter_start_document($xw, '1.0', "UTF-8");
xmlwriter_start_element($xw, 'root');
xmlwriter_start_element_ns($xw, 'ns1', 'child1', 'urn:ns1');
xmlwriter_write_attribute_ns($xw, 'ns1','att1', 'urn:ns1', '<>"\'&');
xmlwriter_write_element($xw, 'chars', "special characters: <>\"'&");
xmlwriter_end_element($xw);
xmlwriter_start_element($xw, 'empty');
xmlwriter_full_end_element($xw);
xmlwriter_full_end_element($xw);
// Force to write and empty the buffer
$output = xmlwriter_flush($xw, true);
print $output;
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root>
   <ns1:child1 ns1:att1="&lt;&gt;&quot;'&amp;" xmlns:ns1="urn:ns1">
      <chars>special characters: &lt;&gt;&quot;'&amp;</chars>
   </ns1:child1>
   <empty></empty>
</root>
