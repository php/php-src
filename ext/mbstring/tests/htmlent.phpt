--TEST--
HTML input/output
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
    ini_set('include_path', __DIR__);
    ?>
--INI--
output_buffering=4096
output_handler=mb_output_handler
zlib.output_compression=
arg_separator.input=x
error_reporting=0
input_encoding=HTML-ENTITIES
output_encoding=HTML-ENTITIES
mbstring.encoding_translation=1
filter.default=unsafe_raw
--FILE--
<?php
// enable output encoding through output handler
//ob_start("mb_output_handler");
// &#64... are must be decoded on input these are not reencoded on output.
// If you see &#64;&#65;&#66; on output this means input encoding fails.
// If you do not see &auml;... on output this means output encoding fails.
// Using UTF-8 internally allows to encode/decode ALL characters.
// &128... will stay as they are since their character codes are above 127
// and they do not have a named entity representation.
?>
<?php echo mb_http_input('l').'>'.mb_internal_encoding().'>'.mb_http_output();?>

<?php mb_parse_str("test=&#38;&#64;&#65;&#66;&#128;&#129;&#130;&auml;&ouml;&uuml;&euro;&lang;&rang;", $test);
print_r($test);
?>
--EXPECT--
HTML-ENTITIES>UTF-8>HTML-ENTITIES
Array
(
    [test] => &@AB&#128;&#129;&#130;&auml;&ouml;&uuml;&euro;&lang;&rang;
)
