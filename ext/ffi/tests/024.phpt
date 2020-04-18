--TEST--
FFI 024: anonymous struct/union
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
    $p = FFI::new("
    struct {
        int a;
        struct {
            int b;
            int c;
        };
        union {
            int d;
            uint32_t e;
        };
        int f;
    }");
    var_dump(FFI::sizeof($p));
    $p->a = 1;
    $p->b = 2;
    $p->c = 3;
    $p->d = 4;
    $p->f = 5;
    var_dump($p);
?>
--EXPECTF--
int(20)
object(FFI\CData:struct <anonymous>)#%d (6) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  ["d"]=>
  int(4)
  ["e"]=>
  int(4)
  ["f"]=>
  int(5)
}
