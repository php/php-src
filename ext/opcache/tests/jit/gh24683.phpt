--TEST--
GH-24683 (Tracing JIT spills a hoisted constant into a live CV)
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die('skip requires 64-bit integers'); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_loop=61
opcache.jit_hot_func=127
opcache.jit_hot_return=8
opcache.jit_hot_side_exit=8
opcache.file_update_protection=0
--FILE--
<?php
// php -d opcache.enable_cli=1 -d opcache.jit=tracing -d opcache.jit_buffer_size=64M repro.php
//
// step() is a small state machine. Case 1 sets $ptr to a constant, uses it to
// load the real pointer, and stores that in $ptr; it then falls through to case 2,
// which reads through $ptr. After the tracing JIT has compiled the loop in
// step(), $ptr sometimes still holds the constant when step() returns.

final class Bus
{
    public int $value = 0;
    public function read(int $address): int { return $this->value; }
}

function step(Bus $bus, array &$state, array &$ram, int $pc): int
{
    $ptr = $state['ptr']; $i = $state['i']; $n = $state['n']; $ret = $state['ret'];
    $c = 0;
    while (true) {
        switch ($pc) {
            case 1:
                $ptr = 0x80080000;
                $a = $ptr - 0x6260;
                if (($a & 3) !== 0) { goto trap; }
                $p = $a & 0x1FFFFFFF;
                $inRam = $p < 0x00800000;
                $x = $inRam ? $ram[$p >> 2] : $bus->read($a);
                $ptr = $x;              // 0x1F8010F4
                $t = $i !== $n;         // unused, but needed to reproduce
                $c += 6;                // unused, but needed to reproduce
                // fall through
            case 2:
                $a = $ptr;
                $p = $a & 0x1FFFFFFF;
                $inRam = $p < 0x00800000;
                $y = $inRam ? $ram[$p >> 2] : $bus->read($a);
                $pc = ($y & 0x7F000000) !== 0 ? 3 : 4;
                continue 2;
            case 3:
                if ($pc === 99) { goto trap; }
                $ret = 1;
                $pc = $ram[0];          // 5
                // fall through
            default:
                goto leave;
        }
    }
    leave:
    $state['ptr'] = $ptr; $state['ret'] = $ret;
    return $pc;
    trap:
    $state['ptr'] = $ptr;
}

$bus = new Bus();
$ram = array_fill(0, 0x80000, 0);
$ram[0] = 5;
$ram[0x79DA0 >> 2] = 0x1F8010F4;

$wrong = 0;
for ($k = 0; $k < 10; $k++) {
    $bus->value = ($k % 128) << 24;
    $state = ['ptr' => 0x1F8010F4, 'i' => 0, 'n' => 0, 'ret' => 0];
    $pc = 2;
    for ($m = 0; $m < 100; $m++) {
        $pc = step($bus, $state, $ram, $pc);
        if ($state['ptr'] !== 0x1F8010F4) {
            printf("k=%d m=%d: \$ptr is 0x%X, expected 0x1F8010F4\n", $k, $m, $state['ptr']);
            $wrong++;
        }
        if ($pc !== 5) {
            break;
        }
        $pc = $state['ret'];
    }
}
echo "wrong: $wrong\n";
--EXPECT--
wrong: 0
