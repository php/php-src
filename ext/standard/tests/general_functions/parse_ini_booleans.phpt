--TEST--
parse_ini_file() boolean operators
--FILE--
<?php

$ini_file = dirname(__FILE__)."/parse_ini_booleans.data";

var_dump(parse_ini_file($ini_file, 1));

echo "Done.\n";

?>
--EXPECT--
array(3) {
  [u"error_reporting values"]=>
  array(6) {
    [u"foo"]=>
    unicode(7) "32767 8"
    [u"error_reporting"]=>
    unicode(5) "32767"
    [u"error_reporting1"]=>
    unicode(4) "4177"
    [u"error_reporting2"]=>
    unicode(5) "32759"
    [u"error_reporting3"]=>
    unicode(5) "32759"
    [u"error_reporting4"]=>
    unicode(5) "32759"
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
