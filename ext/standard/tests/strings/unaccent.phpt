--TEST--
Test trim() function : basic Functionality
--FILE--
<?php
var_dump(unaccent("aâäàåáãAÄÅÁÂÀ"));
var_dump(unaccent("eéêëèEÉÊËÈ"));
var_dump(unaccent("iïîìíıIÍÎÏ"));
var_dump(unaccent("oôöòøóõOÖØÓÔÒÕ"));
var_dump(unaccent("uüûùúUÜÚÛÙ"));
var_dump(unaccent("yÿýYÝ"));
var_dump(unaccent("nñNÑ"));
var_dump(unaccent("cçCÇ"));
var_dump(unaccent("ðdDÐ"));
var_dump(unaccent("var_foo"));
var_dump(unaccent(NULL));
var_dump(unaccent("aâäàåáãAÄÅÁÂÀeéêëèEÉÊËÈiïîìíıIÍÎÏoôöòøóõOÖØÓÔÒÕuüûùúUÜÚÛÙyÿýYÝnñNÑcçCÇðdDÐ"));


?>
--EXPECT--
string(13) "aaaaaaaAAAAAA"
string(10) "eeeeeEEEEE"
string(10) "iiiiiiIIII"
string(14) "oooooooOOOOOOO"
string(10) "uuuuuUUUUU"
string(5) "yyyYY"
string(4) "nnNN"
string(4) "ccCC"
string(4) "ddDD"
string(7) "var_foo"
string(0) ""
string(74) "aaaaaaaAAAAAAeeeeeEEEEEiiiiiiIIIIoooooooOOOOOOOuuuuuUUUUUyyyYYnnNNccCCddDD"
