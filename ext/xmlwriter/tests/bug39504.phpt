--TEST--
bug# 39504 (xmlwriter_write_dtd_entity() creates Attlist tag, not enity)
--FILE--
<?php

$xw = xmlwriter_open_memory();
xmlwriter_start_document($xw, NULL, "UTF-8");
xmlwriter_start_dtd($xw, "root");
xmlwriter_write_dtd_entity($xw, "ent2", "val2");
xmlwriter_end_dtd($xw);
xmlwriter_start_element($xw, "root");
xmlwriter_end_document($xw);
print xmlwriter_flush($xw, true);

?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [<!ENTITY ent2 "val2">]><root/>
