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
  ["warning_count"]=>
  int(1)
  ["warnings"]=>
  array(1) {
    [6]=>
    string(29) "Double timezone specification"
  }
  ["error_count"]=>
  int(1)
  ["errors"]=>
  array(1) {
    [0]=>
    string(47) "The timezone could not be found in the database"
  }
}
array(4) {
  ["warning_count"]=>
  int(1)
  ["warnings"]=>
  array(1) {
    [6]=>
    string(29) "Double timezone specification"
  }
  ["error_count"]=>
  int(1)
  ["errors"]=>
  array(1) {
    [0]=>
    string(47) "The timezone could not be found in the database"
  }
}
