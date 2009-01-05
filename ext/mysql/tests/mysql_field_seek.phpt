--TEST--
mysql_field_seek()
--SKIPIF--
<?php 
require_once('skipif.inc'); 
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

$tmp    = NULL;
$link   = NULL;

if (!is_null($tmp = @mysql_field_seek()))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (!is_null($tmp = @mysql_field_seek($link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');
if (!$res = mysql_query("SELECT id, label FROM test ORDER BY id LIMIT 1", $link)) {
	printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));
}

var_dump(mysql_field_seek($res, -1));
var_dump(mysql_fetch_field($res));
var_dump(mysql_field_seek($res, 0));
var_dump(mysql_fetch_field($res));
var_dump(mysql_field_seek($res, 1));
var_dump(mysql_fetch_field($res));
var_dump(mysql_field_seek($res, 2));
var_dump(mysql_fetch_field($res));

mysql_free_result($res);

var_dump(mysql_field_seek($res, 0));

mysql_close($link);
print "done!";
?>
--EXPECTF--
Warning: mysql_field_seek(): Field -1 is invalid for MySQL result index %d in %s on line %d
bool(false)
object(stdClass)#%d (13) {
  [u"name"]=>
  unicode(2) "id"
  [u"table"]=>
  unicode(4) "test"
  [u"def"]=>
  unicode(0) ""
  [u"max_length"]=>
  int(1)
  [u"not_null"]=>
  int(1)
  [u"primary_key"]=>
  int(1)
  [u"multiple_key"]=>
  int(0)
  [u"unique_key"]=>
  int(0)
  [u"numeric"]=>
  int(1)
  [u"blob"]=>
  int(0)
  [u"type"]=>
  unicode(3) "int"
  [u"unsigned"]=>
  int(0)
  [u"zerofill"]=>
  int(0)
}
bool(true)
object(stdClass)#%d (13) {
  [u"name"]=>
  unicode(2) "id"
  [u"table"]=>
  unicode(4) "test"
  [u"def"]=>
  unicode(0) ""
  [u"max_length"]=>
  int(1)
  [u"not_null"]=>
  int(1)
  [u"primary_key"]=>
  int(1)
  [u"multiple_key"]=>
  int(0)
  [u"unique_key"]=>
  int(0)
  [u"numeric"]=>
  int(1)
  [u"blob"]=>
  int(0)
  [u"type"]=>
  unicode(3) "int"
  [u"unsigned"]=>
  int(0)
  [u"zerofill"]=>
  int(0)
}
bool(true)
object(stdClass)#%d (13) {
  [u"name"]=>
  unicode(5) "label"
  [u"table"]=>
  unicode(4) "test"
  [u"def"]=>
  unicode(0) ""
  [u"max_length"]=>
  int(1)
  [u"not_null"]=>
  int(0)
  [u"primary_key"]=>
  int(0)
  [u"multiple_key"]=>
  int(0)
  [u"unique_key"]=>
  int(0)
  [u"numeric"]=>
  int(0)
  [u"blob"]=>
  int(0)
  [u"type"]=>
  unicode(6) "string"
  [u"unsigned"]=>
  int(0)
  [u"zerofill"]=>
  int(0)
}

Warning: mysql_field_seek(): Field %d is invalid for MySQL result index %d in %s on line %d
bool(false)
bool(false)

Warning: mysql_field_seek(): %d is not a valid MySQL result resource in %s on line %d
bool(false)
done!
