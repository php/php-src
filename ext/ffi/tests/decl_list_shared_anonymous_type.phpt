--TEST--
FFI: declarator lists sharing an anonymous struct, union or enum type
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php

$ffi = FFI::cdef(<<<'C'
typedef struct { int a; } T, *P, A[2];
typedef union { int i; char c; } U1, U2;
typedef enum { E_A, E_B } E1, E2;
struct S { struct { int x; } p, q; enum { F_A } e, f; };
static struct { int z; } s1, *s2;
C);

$t = $ffi->new("T");
$t->a = 1;
$a = $ffi->new("A");
$a[1]->a = 2;
$p = $ffi->new("P");
$p = FFI::addr($t);
echo $t->a, $a[1]->a, $p->a, PHP_EOL;

$u = $ffi->new("U2");
$u->i = 3;
echo $u->i, PHP_EOL;

echo FFI::sizeof($ffi->new("E1")), FFI::sizeof($ffi->new("E2")), PHP_EOL;

$s = $ffi->new("struct S");
$s->p->x = 4;
$s->q->x = 5;
echo $s->p->x, $s->q->x, PHP_EOL;

unset($ffi, $t, $a, $p, $u, $s);
echo "Done", PHP_EOL;

?>
--EXPECT--
121
3
44
45
Done
