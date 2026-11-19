--TEST--
iconv encoding deprecated
--EXTENSIONS--
iconv
--FILE--
<?php

iconv_set_encoding("internal_encoding", "UTF-8");
iconv_set_encoding("output_encoding",   "UTF-8");
iconv_set_encoding("input_encoding",    "UTF-8");

var_dump( iconv_get_encoding('internal_encoding') );
var_dump( iconv_get_encoding('output_encoding')   );
var_dump( iconv_get_encoding('input_encoding')    );
var_dump( iconv_get_encoding('all')               );

?>
--EXPECTF--
Deprecated: Function iconv_set_encoding() is deprecated since 8.5, use internal_encoding, input_encoding, and output_encoding INI settings instead in %s on line %d

Deprecated: iconv_set_encoding(): Use of iconv.internal_encoding is deprecated in %s on line %d

Deprecated: Function iconv_set_encoding() is deprecated since 8.5, use internal_encoding, input_encoding, and output_encoding INI settings instead in %s on line %d

Deprecated: iconv_set_encoding(): Use of iconv.output_encoding is deprecated in %s on line %d

Deprecated: Function iconv_set_encoding() is deprecated since 8.5, use internal_encoding, input_encoding, and output_encoding INI settings instead in %s on line %d

Deprecated: iconv_set_encoding(): Use of iconv.input_encoding is deprecated in %s on line %d

Deprecated: Function iconv_get_encoding() is deprecated since 8.5, use internal_encoding, input_encoding, and output_encoding INI settings instead in %s on line %d
string(5) "UTF-8"

Deprecated: Function iconv_get_encoding() is deprecated since 8.5, use internal_encoding, input_encoding, and output_encoding INI settings instead in %s on line %d
string(5) "UTF-8"

Deprecated: Function iconv_get_encoding() is deprecated since 8.5, use internal_encoding, input_encoding, and output_encoding INI settings instead in %s on line %d
string(5) "UTF-8"

Deprecated: Function iconv_get_encoding() is deprecated since 8.5, use internal_encoding, input_encoding, and output_encoding INI settings instead in %s on line %d
array(3) {
  ["input_encoding"]=>
  string(5) "UTF-8"
  ["output_encoding"]=>
  string(5) "UTF-8"
  ["internal_encoding"]=>
  string(5) "UTF-8"
}
