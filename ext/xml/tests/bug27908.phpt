--TEST--
Bug #27908 (default handler not being called)
--SKIPIF--
<?php
require_once("skipif.inc");
?>
--FILE--
<?php

function x_default_handler($xp,$data) 
{ 
    echo "x_default_handler $data\n"; 
} 
$xp = xml_parser_create(); 
xml_set_default_handler($xp,'x_default_handler'); 
xml_parse($xp, '<root></root>',TRUE); 
xml_parser_free($xp); 
echo "Done\n";
?>
--EXPECT--
x_default_handler <root>
x_default_handler </root>
Done
