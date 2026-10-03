--TEST--
GH-22559 (Tracing JIT: bitwise-NOT high bits leak past a mask into a typed int property)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=tracing
; run-tests forces jit_hot_side_exit=1, which compiles side traces so eagerly
; that the buggy trace never forms — override back to the default so the bug shows.
opcache.jit_hot_side_exit=8
--FILE--
<?php
final class C {
    /** @var int[] */ public array $t = [];
    public int $a = 0;
    public int $f = 0;
    public function __construct() { for ($i = 0; $i < 256; $i++) $this->t[$i] = $i & 0xA8; }
    public function add8(int $value, int $carry): void {
        $a = $this->a;
        $total = $a + $value + $carry;
        $result = $total & 0xFF;
        $this->f = $this->t[$result]
            | (($total & 0x100) ? 0x01 : 0)
            | (((($a & 0x0F) + ($value & 0x0F) + $carry) & 0x10) ? 0x10 : 0)
            | ((~($a ^ $value) & ($a ^ $result) & 0x80) ? 0x04 : 0);
        $this->a = $result;
    }
}
$c = new C();
for ($i = 0; $i < 100000; $i++) {
    $c->a = $i & 0xFF;
    $c->add8(($i >> 8) & 0xFF, 0);
    if (($c->f & ~0xFF) !== 0) {
        printf("MISCOMPILED at i=%d: \$f = %d (0x%X)\n", $i, $c->f, $c->f);
        break;
    }
}
echo "done\n";
?>
--EXPECT--
done
