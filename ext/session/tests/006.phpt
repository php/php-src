--TEST--
References between variables in sessions
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
register_globals=1
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);

session_id("abtest");
session_start();

class a {
    var $test = "hallo";
}
 
class b {
    var $a;
    function b(&$a) {
        $this->a = &$a;
    }
}
 
$a = new a();
$b = new b($a);

echo "original values:\n";
var_dump($a,$b);

session_register("a");
session_register("b");
session_write_close();

session_unregister("a");
session_unregister("b");

session_start();

echo "values after session:\n";
var_dump($a,$b);
?>
--EXPECT--
original values:
object(a)(1) {
  ["test"]=>
  string(5) "hallo"
}
object(b)(1) {
  ["a"]=>
  &object(a)(1) {
    ["test"]=>
    string(5) "hallo"
  }
}
values after session:
object(a)(1) {
  ["test"]=>
  string(5) "hallo"
}
object(b)(1) {
  ["a"]=>
  &object(a)(1) {
    ["test"]=>
    string(5) "hallo"
  }
}
