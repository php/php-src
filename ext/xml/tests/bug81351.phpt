--TEST--
Bug #81351 (xml_parse may fail, but has no error code)
--EXTENSIONS--
xml
--FILE--
<?php
$xml = <<<XML
<?xml version="1.0" encoding="utf-8"?>
<soap:Envelope xmlns:soap="http://www.w3.org/2003/05/soap-envelope" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
 <soap:Body>
  <X xmlns="example.org">
XML;

$parser = xml_parser_create_ns('UTF-8');
$success = xml_parse($parser, $xml, false);
$code = xml_get_error_code($parser);
$error = xml_error_string($code);
echo "xml_parse returned $success, xml_get_error_code = $code, xml_error_string = $error\r\n";
$success = xml_parse($parser, 'Y>', true);
$code = xml_get_error_code($parser);
$error = xml_error_string($code);
echo "xml_parse returned $success, xml_get_error_code = $code, xml_error_string = $error\r\n";
?>
--EXPECT--
xml_parse returned 1, xml_get_error_code = 0, xml_error_string = No error
xml_parse returned 0, xml_get_error_code = 5, xml_error_string = Invalid document end
