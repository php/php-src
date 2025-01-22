--TEST--
JIT ASSIGN_OBJ: Typed & not-typed property 
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
interface I {
}
abstract class C1 implements I {
    public function __construct($x) {
        $this->x = $x;
    }
}
class C2 extends C1 {
    public $x = 0;
}
class C3 extends C1 {
    public int $x = 0;
}
$o = new C2("abcd");
var_dump($o->x);
$o = new C3(42);
var_dump($o->x);
$o = new C3("abcd");
var_dump($o->x);
?>
--EXPECTF--
string(4) "abcd"
int(42)

Fatal error: Uncaught TypeError: Cannot assign string to property C3::$x of type int in %sassign_obj_005.php:6
Stack trace:
#0 %sassign_obj_005.php(19): C1->__construct('abcd')
#1 {main}
  thrown in %sassign_obj_005.php on line 6
