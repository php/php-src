<?php
dl('xmlwriter.so');

$xw = xmlwriter_open_memory();
xmlwriter_set_indent($xw, 1);
$res = xmlwriter_set_indent_string($xw, ' ');

xmlwriter_start_document($xw, '1.0', 'UTF-8');

// A first element
xmlwriter_start_element($xw, 'tag1');

// Attribute 'att1' for element 'tag1'
xmlwriter_start_attribute($xw, 'att1');
xmlwriter_text($xw, 'valueofatt1');
xmlwriter_end_attribute($xw);

xmlwriter_text($xw, utf8_encode('This is a sample text, ä'));
xmlwriter_end_element($xw); // tag1


$res = xmlwriter_start_comment($xw);
xmlwriter_text($xw, "Demo text comment");
$res = xmlwriter_end_comment($xw);

xmlwriter_end_document($xw);
$out = xmlwriter_output_memory($xw, 0);

echo $out;

// flush the xml buffer using optional
// flust argument, default is 1
$out = xmlwriter_output_memory($xw, 1);
echo $out;


$out = xmlwriter_output_memory($xw);
echo $out;

