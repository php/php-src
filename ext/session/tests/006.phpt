--TEST--
correct instantiation of references between variables in sessions
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.use_strict_mode=0
session.cache_limiter=
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
error_reporting(E_ALL);

session_id("abtest");
session_start();

class a {
    public $test = "hallo";
}
 
class b {
    public $a;
    function b(&$a) {
        $this->a = &$a;
    }
}
 
$a = new a();
$b = new b($a);

echo "original values:\n";
var_dump($a,$b);

$_SESSION["a"] = $a;
$_SESSION["b"] = $b;
session_write_close();

unset($_SESSION["a"], $_SESSION["b"]);

session_start();

echo "values after session:\n";
var_dump($a,$b);
?>
--EXPECTF--
original values:
object(a)#%d (1) {
  ["test"]=>
  string(5) "hallo"
}
object(b)#%d (1) {
  ["a"]=>
  &object(a)#%d (1) {
    ["test"]=>
    string(5) "hallo"
  }
}
values after session:
object(a)#%d (1) {
  ["test"]=>
  string(5) "hallo"
}
object(b)#%d (1) {
  ["a"]=>
  &object(a)#%d (1) {
    ["test"]=>
    string(5) "hallo"
  }
}
