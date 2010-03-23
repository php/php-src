--TEST--
correct instantiation of references between variables in sessions
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
register_globals=1
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

session_register("a");
session_register("b");
session_write_close();

session_unregister("a");
session_unregister("b");

session_start();

echo "values after session:\n";
var_dump($a,$b);
?>
--EXPECTF--
Warning: Directive 'register_globals' is deprecated in PHP 5.3 and greater in Unknown on line 0
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

Deprecated: Function session_register() is deprecated in %s on line %d

Deprecated: Function session_register() is deprecated in %s on line %d

Deprecated: Function session_unregister() is deprecated in %s on line %d

Deprecated: Function session_unregister() is deprecated in %s on line %d
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

