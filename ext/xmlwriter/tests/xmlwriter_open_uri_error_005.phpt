--TEST--
xmlwriter_open_uri with file://localhost/
--EXTENSIONS--
xmlwriter
--FILE--
<?php
var_dump(xmlwriter_open_uri('file://localhost/'));
?>
--CREDITS--
Koen Kuipers koenk82@gmail.com
Theo van der Zee
#Test Fest Utrecht 09-05-2009
--EXPECTF--
Warning: xmlwriter_open_uri(): Unable to resolve file path in %s on line %d
bool(false)
