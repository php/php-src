--TEST--
XMLWriter: libxml2 XML Writer DTD Element & Attlist
--EXTENSIONS--
xmlwriter
--FILE--
<?php

$xw = xmlwriter_open_memory();
xmlwriter_set_indent($xw, TRUE);
xmlwriter_start_document($xw, NULL, "UTF-8");
xmlwriter_start_dtd_entity($xw, "ent", false);
xmlwriter_text($xw, "val");
xmlwriter_end_dtd_entity($xw);
xmlwriter_write_dtd_entity($xw, "ent2", "val2");
xmlwriter_write_dtd_element($xw, 'sxe', '(elem1+, elem11, elem22*)');
xmlwriter_write_dtd_attlist($xw, 'sxe', 'id     CDATA  #implied');
xmlwriter_start_dtd_element($xw, 'elem1');
xmlwriter_text($xw, 'elem2*');
xmlwriter_end_dtd_element($xw);
xmlwriter_start_dtd_attlist($xw, 'elem1');
xmlwriter_text($xw, "attr1  CDATA  #required\n");
xmlwriter_text($xw, 'attr2  CDATA  #implied');
xmlwriter_end_dtd_attlist($xw);
xmlwriter_end_document($xw);
// Force to write and empty the buffer
$output = xmlwriter_flush($xw, true);
print $output;
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<!ENTITY ent "val">
<!ENTITY ent2 "val2">
<!ELEMENT sxe (elem1+, elem11, elem22*)>
<!ATTLIST sxe id     CDATA  #implied>
<!ELEMENT elem1 elem2*>
<!ATTLIST elem1 attr1  CDATA  #required
attr2  CDATA  #implied>
