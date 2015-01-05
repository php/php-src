--TEST--
mysql_fetch_object()
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

if (!is_null($tmp = @mysql_fetch_object()))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (false !== ($tmp = @mysql_fetch_object($link)))
	printf("[002] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');
if (!$res = mysql_query("SELECT id AS ID, label FROM test AS TEST ORDER BY id LIMIT 5", $link)) {
	printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));
}

var_dump(mysql_fetch_object($res));

class mysql_fetch_object_test {

	public $a = null;
	public $b = null;

	public function toString() {
		var_dump($this);
	}
}

var_dump(mysql_fetch_object($res, 'mysql_fetch_object_test'));

class mysql_fetch_object_construct extends mysql_fetch_object_test {

	public function __construct($a, $b) {
		$this->a = $a;
		$this->b = $b;
	}

}

var_dump(mysql_fetch_object($res, 'mysql_fetch_object_construct', null));
var_dump(mysql_fetch_object($res, 'mysql_fetch_object_construct', array('a')));
var_dump(mysql_fetch_object($res, 'mysql_fetch_object_construct', array('a', 'b')));
var_dump(mysql_fetch_object($res, 'mysql_fetch_object_construct', array('a', 'b', 'c')));
var_dump(mysql_fetch_object($res, 'mysql_fetch_object_construct', "no array and not null"));
var_dump(mysql_fetch_object($res));
var_dump(mysql_fetch_object($res, 'mysql_fetch_object_construct', array('a', 'b')));

class mysql_fetch_object_private_construct {
	private function __construct($a, $b) {
		var_dump($a);
	}
}
var_dump(mysql_fetch_object($res, 'mysql_fetch_object_private_construct', array('a', 'b')));

mysql_free_result($res);

if (!$res = mysql_query("SELECT id AS ID, label FROM test AS TEST", $link)) {
	printf("[009] [%d] %s\n", mysql_errno($link), mysql_error($link));
}

mysql_free_result($res);

var_dump(mysql_fetch_object($res));

// Fatal error, script execution will end
var_dump(mysql_fetch_object($res, 'this_class_does_not_exist'));

mysql_close($link);
print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
object(stdClass)#%d (2) {
  [%u|b%"ID"]=>
  %unicode|string%(1) "1"
  [%u|b%"label"]=>
  %unicode|string%(1) "a"
}
object(mysql_fetch_object_test)#%d (4) {
  [%u|b%"a"]=>
  NULL
  [%u|b%"b"]=>
  NULL
  [%u|b%"ID"]=>
  %unicode|string%(1) "2"
  [%u|b%"label"]=>
  %unicode|string%(1) "b"
}

Warning: Missing argument 1 for mysql_fetch_object_construct::__construct() in %s on line %d

Warning: Missing argument 2 for mysql_fetch_object_construct::__construct() in %s on line %d

Notice: Undefined variable: a in %s on line %d

Notice: Undefined variable: b in %s on line %d
object(mysql_fetch_object_construct)#%d (4) {
  [%u|b%"a"]=>
  NULL
  [%u|b%"b"]=>
  NULL
  [%u|b%"ID"]=>
  %unicode|string%(1) "3"
  [%u|b%"label"]=>
  %unicode|string%(1) "c"
}

Warning: Missing argument 2 for mysql_fetch_object_construct::__construct() in %s on line %d

Notice: Undefined variable: b in %s on line %d
object(mysql_fetch_object_construct)#%d (4) {
  [%u|b%"a"]=>
  %unicode|string%(1) "a"
  [%u|b%"b"]=>
  NULL
  [%u|b%"ID"]=>
  %unicode|string%(1) "4"
  [%u|b%"label"]=>
  %unicode|string%(1) "d"
}
object(mysql_fetch_object_construct)#%d (4) {
  [%u|b%"a"]=>
  %unicode|string%(1) "a"
  [%u|b%"b"]=>
  %unicode|string%(1) "b"
  [%u|b%"ID"]=>
  %unicode|string%(1) "5"
  [%u|b%"label"]=>
  %unicode|string%(1) "e"
}
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: mysql_fetch_object(): supplied resource is not a valid MySQL result resource in %s on line %d
bool(false)

Fatal error: Class 'this_class_does_not_exist' not found in %s on line %d
