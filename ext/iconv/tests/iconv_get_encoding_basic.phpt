--TEST--
iconv_get_encoding() parameter tests
--CREDITS--
Oystein Rose <orose@redpill-linpro.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php if (!extension_loaded("iconv")) { echo 'skip extension not available'; } ?>
--FILE--
<?php

iconv_set_encoding("internal_encoding", "UTF-8");
iconv_set_encoding("output_encoding",   "UTF-8");
iconv_set_encoding("input_encoding",    "UTF-8");

var_dump( iconv_get_encoding('internal_encoding') );
var_dump( iconv_get_encoding('output_encoding')   );
var_dump( iconv_get_encoding('input_encoding')    );
var_dump( iconv_get_encoding('all')               );
var_dump( iconv_get_encoding('foo')               );
var_dump( iconv_get_encoding()                    );



iconv_set_encoding("internal_encoding", "ISO-8859-1");
iconv_set_encoding("output_encoding",   "ISO-8859-1");
iconv_set_encoding("input_encoding",    "ISO-8859-1");

var_dump( iconv_get_encoding('internal_encoding') );
var_dump( iconv_get_encoding('output_encoding')   );
var_dump( iconv_get_encoding('input_encoding')    );
var_dump( iconv_get_encoding('all')               );
var_dump( iconv_get_encoding('foo')               );
var_dump( iconv_get_encoding()                    );

?>
--EXPECT--
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
array(3) {
  ["input_encoding"]=>
  string(5) "UTF-8"
  ["output_encoding"]=>
  string(5) "UTF-8"
  ["internal_encoding"]=>
  string(5) "UTF-8"
}
bool(false)
array(3) {
  ["input_encoding"]=>
  string(5) "UTF-8"
  ["output_encoding"]=>
  string(5) "UTF-8"
  ["internal_encoding"]=>
  string(5) "UTF-8"
}
string(10) "ISO-8859-1"
string(10) "ISO-8859-1"
string(10) "ISO-8859-1"
array(3) {
  ["input_encoding"]=>
  string(10) "ISO-8859-1"
  ["output_encoding"]=>
  string(10) "ISO-8859-1"
  ["internal_encoding"]=>
  string(10) "ISO-8859-1"
}
bool(false)
array(3) {
  ["input_encoding"]=>
  string(10) "ISO-8859-1"
  ["output_encoding"]=>
  string(10) "ISO-8859-1"
  ["internal_encoding"]=>
  string(10) "ISO-8859-1"
}
