--TEST--
libxml_use_internal_errors(false) clears the last error
--EXTENSIONS--
libxml
dom
--FILE--
<?php

libxml_use_internal_errors(true);

$document = new DOMDocument();
$document->loadXML('<a><b></a>');

var_dump(count(libxml_get_errors()) > 0);
var_dump(libxml_get_last_error() instanceof LibXMLError);

var_dump(libxml_use_internal_errors(false));

var_dump(libxml_get_errors());
var_dump(libxml_get_last_error());

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
array(0) {
}
bool(false)
