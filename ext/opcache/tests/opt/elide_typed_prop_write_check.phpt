--TEST--
Elide the type check on a typed property write when the value provably satisfies the property type
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--FILE--
<?php
class C {
    public int $i = 0;
    public float $f = 0.0;
    public function exact(int $i): void { $this->i = $i; }
    public function coerce(int $x): void { $this->f = $x; }
}
echo "done\n";
?>
--EXPECTF--
$_main:
     ; (lines=2, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s:1-10
0000 ECHO string("done\n")
0001 RETURN int(1)

C::exact:
     ; (lines=4, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:5-5
0000 CV0($i) = RECV 1
0001 ASSIGN_OBJ THIS string("i") (skip type check)
0002 OP_DATA CV0($i)
0003 RETURN null

C::coerce:
     ; (lines=4, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:6-6
0000 CV0($x) = RECV 1
0001 ASSIGN_OBJ THIS string("f")
0002 OP_DATA CV0($x)
0003 RETURN null
done
