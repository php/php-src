--TEST--
GH-22763: JIT fails to clear ZREG_TYPE_ONLY after setting reg
--FILE--
<?php

function findMiddleSnake(array $a, int $aLo, int $aHi, array $b, int $bLo, int $bHi): array
{
    $n          = $aHi - $aLo;
    $m          = $bHi - $bLo;
    $delta      = $n - $m;
    $deltaIsOdd = ($delta & 1) !== 0;
    $offset     = 4;
    $size       = 2 * 3 + 2;
    $vf         = array_fill(0, $size, 0);
    $vb         = array_fill(0, $size, 0);

    $d = 2;
    $x = 0;

    for ($k = -$d; $k <= $d; $k += 2) {
        if ($k === -$d || ($k !== $d && $vb[$offset + $k - 1] < $vb[$offset + $k + 1])) {
            $x = $vb[$offset + $k + 1];
            var_dump($x);
        } else {
            $x = $vb[$offset + $k - 1] + 1;
        }

        $y  = $x - $k;
        $xs = $x;
        $ys = $y;

        while ($x < $n && $y < $m && $a[$aLo + $n - 1 - $x] === $b[$bLo + $m - 1 - $y]) {
            $x++;
            $y++;
        }

        $vb[$offset + $k] = $x;

        if (!$deltaIsOdd) {
            $kf = $delta - $k;

            if ($kf >= -$d && $kf <= $d && $vf[$offset + $kf] + $vb[$offset + $k] >= $n) {
                return [$n - $x, $m - $y, $n - $xs, $m - $ys];
            }
        }
    }

    return [];
}

$from = [3,2,1];
$to = [1,99,3];
findMiddleSnake($from, 0, count($from), $to, 0, count($to));
?>
--EXPECTF--
int(0)

Warning: Undefined array key 3 in %s on line %d
