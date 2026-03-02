--TEST--
xml_parser_free() deprecation message
--EXTENSIONS--
xml
--FILE--
<?php

$xp = xml_parser_create();
xml_parse($xp, '<root></root>',TRUE);
xml_parser_free($xp);
echo "Done\n";
?>
--EXPECTF--
Deprecated: Function xml_parser_free() is deprecated since 8.5, as it has no effect since PHP 8.0 in %s on line %d
Done
