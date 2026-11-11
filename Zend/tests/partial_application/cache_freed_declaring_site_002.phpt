--TEST--
PFA in eval(): request cache must not outlive the freed declaring op_array
--FILE--
<?php
function f($a = 'A', $b = 'B', $c = 'C', $d = 'D', $e = 'E', $f = 'F', $g = 'G') { return "$a|$b|$c|$d|$e|$f|$g"; }

$n = 0;
for ($round = 0; $round < 2; $round++) {
    for ($argc = 1; $argc <= 7; $argc++) {
        for ($mask = 1; $mask < (1 << $argc); $mask++) {
            $args = [];
            $expected = ['A', 'B', 'C', 'D', 'E', 'F', 'G'];
            $placeholders = 0;
            for ($i = 0; $i < $argc; $i++) {
                if ($mask & (1 << $i)) {
                    $args[] = '?';
                    $expected[$i] = ++$placeholders;
                } else {
                    $args[] = "'v$i'";
                    $expected[$i] = "v$i";
                }
            }
            $shape = 'f(' . implode(', ', $args) . ')';
            $n++;
            $pfa = eval("return $shape;");
            $result = $pfa(...range(1, $placeholders));
            if ($result !== implode('|', $expected)) {
                echo "$shape: got $result\n";
            }
        }
    }
}
echo "Done\n";
?>
--EXPECT--
Done
