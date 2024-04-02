--TEST--
Bug #46699: (xml_parse crash when parser is namespace aware)
--EXTENSIONS--
xml
--SKIPIF--
<?php
if (! @xml_parser_create_ns('ISO-8859-1')) { die("skip xml_parser_create_ns is not supported on this platform");}
?>
--FILE--
<?php
function defaultfunc($parser, $data)
{
echo $data;
}

$xml = <<<HERE
<a xmlns="http://example.com/foo"
    xmlns:bar="http://example.com/bar">
  <bar:b foo="bar">1</bar:b>
  <bar:c bar:nix="null" foo="bar">2</bar:c>
</a>
HERE;

$parser = xml_parser_create_ns("ISO-8859-1","@");
xml_set_default_handler($parser,'defaultfunc');
xml_parser_set_option($parser,XML_OPTION_CASE_FOLDING,0);
xml_parse($parser, $xml);
xml_parser_free($parser);
?>
--EXPECTF--
<a xmlns="http://example.com/foo"%axmlns:bar="http://example.com/bar">
  <bar:b foo="bar">1</bar:b>
  <bar:c bar:nix="null" foo="bar">2</bar:c>
</a>
