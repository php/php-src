--TEST--
Bug #43808 (date_create never fails (even when it should))
--FILE--
<?php
$date = date_create('asdfasdf');

if ($date instanceof DateTime) {
	echo "this is wrong, should be bool";
}

var_dump( $date );
var_dump( DateTime::getLastErrors() );
var_dump( date_get_last_errors() );
?>
--EXPECT--
bool(false)
array(4) {
  [u"warning_count"]=>
  int(1)
  [u"warnings"]=>
  array(1) {
    [6]=>
    unicode(29) "Double timezone specification"
  }
  [u"error_count"]=>
  int(1)
  [u"errors"]=>
  array(1) {
    [0]=>
    unicode(47) "The timezone could not be found in the database"
  }
}
array(4) {
  [u"warning_count"]=>
  int(1)
  [u"warnings"]=>
  array(1) {
    [6]=>
    unicode(29) "Double timezone specification"
  }
  [u"error_count"]=>
  int(1)
  [u"errors"]=>
  array(1) {
    [0]=>
    unicode(47) "The timezone could not be found in the database"
  }
}
