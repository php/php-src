--TEST--
FFI 032: bit-fields access
--EXTENSIONS--
ffi
--INI--
ffi.enable=1
--FILE--
<?php
$p = FFI::cdef()->new("
    union {
        struct __attribute__((packed)) {
            int a:2;
            unsigned long long b:64;
            int c:3;
            unsigned int d:3;
        } s;
    uint8_t i[9];
}");
var_dump(FFI::sizeof($p));
for ($i = -5; $i < 9; $i++) {
    $p->s->c = $i;
    $p->s->d = $i;
    echo "$i => 3-bit int {$p->s->c}, 3-bit uint {$p->s->d}\n";
}
$p->s->a = 0;
$p->s->c = 0;
$p->s->d = 0;
$p->s->b = 0x7fffffff;
echo "0x";
for ($i = 9; $i > 0;) {
    printf("%02x", $p->i[--$i]);
}
echo "\n";
?>
ok
--EXPECT--
int(9)
-5 => 3-bit int 3, 3-bit uint 3
-4 => 3-bit int -4, 3-bit uint 4
-3 => 3-bit int -3, 3-bit uint 5
-2 => 3-bit int -2, 3-bit uint 6
-1 => 3-bit int -1, 3-bit uint 7
0 => 3-bit int 0, 3-bit uint 0
1 => 3-bit int 1, 3-bit uint 1
2 => 3-bit int 2, 3-bit uint 2
3 => 3-bit int 3, 3-bit uint 3
4 => 3-bit int -4, 3-bit uint 4
5 => 3-bit int -3, 3-bit uint 5
6 => 3-bit int -2, 3-bit uint 6
7 => 3-bit int -1, 3-bit uint 7
8 => 3-bit int 0, 3-bit uint 0
0x0000000001fffffffc
ok
