--TEST--
PDO_DBLIB: Return date as DateTime object
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
$sql = "SELECT convert(datetime, '20171017 10:22:44.123') AS [d]";
date_default_timezone_set('UTC');

//default = "", expected = ISO format
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

//"millisecond", expected = ISO format + ms
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, "millisecond");
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

//"object", expected = DateTime Object + us
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, "object");
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

//raw = "", expected = freetds format
//FIXME: this test assumes we're using freetds default "date format" = %b %e %Y %I:%M:%S:%z%p
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, "raw");
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

//anything else, expected = DateTime::format() formated string
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, "Y=m=d H_i_s_u");
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
$stmt = $db->query($sql);
var_dump($stmt->fetch(PDO::FETCH_ASSOC));
$stmt->closeCursor();

//set to null is the same as default
$db->setAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT, null);
var_dump($db->getAttribute(PDO::DBLIB_ATTR_DATETIME_FORMAT));
?>
--EXPECT--
string(0) ""
array(1) {
  ["d"]=>
  string(19) "2017-10-17 10:22:44"
}
string(11) "millisecond"
array(1) {
  ["d"]=>
  string(23) "2017-10-17 10:22:44.123"
}
string(6) "object"
array(1) {
  ["d"]=>
  object(DateTime)#3 (3) {
    ["date"]=>
    string(26) "2017-10-17 10:22:44.123000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(3) "UTC"
  }
}
string(3) "raw"
array(1) {
  ["d"]=>
  string(26) "Oct 17 2017 10:22:44:123AM"
}
string(13) "Y=m=d H_i_s_u"
array(1) {
  ["d"]=>
  string(26) "2017=10=17 10_22_44_123000"
}
string(0) ""
