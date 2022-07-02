--TEST--
XMLWriter: libxml2 XML Writer, membuffer, flush
--EXTENSIONS--
xmlwriter
--FILE--
<?php

$xw = xmlwriter_open_memory();
xmlwriter_start_document($xw, '1.0', 'UTF-8');
xmlwriter_start_element($xw, "tag1");
xmlwriter_end_document($xw);

// Force to write and empty the buffer
echo xmlwriter_flush($xw, true);
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<tag1/>
