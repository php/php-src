--TEST--
parse_ini_file() boolean operators
--FILE--
<?php

$ini_file = dirname(__FILE__)."/parse_ini_booleans.data";

var_dump(parse_ini_file($ini_file, 1));

echo "Done.\n";

?>
--EXPECTF--	
array(3) {
  ["error_reporting values"]=>
  array(6) {
    ["foo"]=>
    string(6) "8191 8"
    ["error_reporting"]=>
    string(4) "8191"
    ["error_reporting1"]=>
    string(4) "4177"
    ["error_reporting2"]=>
    string(4) "8183"
    ["error_reporting3"]=>
    string(4) "8183"
    ["error_reporting4"]=>
    string(4) "8183"
  }
  ["true or false"]=>
  array(8) {
    ["bool_true"]=>
    string(1) "1"
    ["bool_yes"]=>
    string(1) "1"
    ["bool_on"]=>
    string(1) "1"
    ["bool_false"]=>
    string(0) ""
    ["bool_off"]=>
    string(0) ""
    ["bool_no"]=>
    string(0) ""
    ["bool_none"]=>
    string(0) ""
    ["bool_null"]=>
    string(0) ""
  }
  ["strings"]=>
  array(8) {
    ["string_true"]=>
    string(4) "true"
    ["string_yes"]=>
    string(4) " yes"
    ["string_on"]=>
    string(5) "  on "
    ["string_false"]=>
    string(5) "false"
    ["string_off"]=>
    string(4) "Off "
    ["string_no"]=>
    string(4) "No	 "
    ["string_none"]=>
    string(5) " NoNe"
    ["string_null"]=>
    string(4) "NULl"
  }
}
Done.
--UEXPECTF--	
array(3) {
  [u"error_reporting values"]=>
  array(6) {
    [u"foo"]=>
    unicode(6) "8191 8"
    [u"error_reporting"]=>
    unicode(4) "8191"
    [u"error_reporting1"]=>
    unicode(4) "4177"
    [u"error_reporting2"]=>
    unicode(4) "8183"
    [u"error_reporting3"]=>
    unicode(4) "8183"
    [u"error_reporting4"]=>
    unicode(4) "8183"
  }
  [u"true or false"]=>
  array(8) {
    [u"bool_true"]=>
    unicode(1) "1"
    [u"bool_yes"]=>
    unicode(1) "1"
    [u"bool_on"]=>
    unicode(1) "1"
    [u"bool_false"]=>
    unicode(0) ""
    [u"bool_off"]=>
    unicode(0) ""
    [u"bool_no"]=>
    unicode(0) ""
    [u"bool_none"]=>
    unicode(0) ""
    [u"bool_null"]=>
    unicode(0) ""
  }
  [u"strings"]=>
  array(8) {
    [u"string_true"]=>
    unicode(4) "true"
    [u"string_yes"]=>
    unicode(4) " yes"
    [u"string_on"]=>
    unicode(5) "  on "
    [u"string_false"]=>
    unicode(5) "false"
    [u"string_off"]=>
    unicode(4) "Off "
    [u"string_no"]=>
    unicode(4) "No	 "
    [u"string_none"]=>
    unicode(5) " NoNe"
    [u"string_null"]=>
    unicode(4) "NULl"
  }
}
Done.
