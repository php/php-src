--TEST--
PFA in included file: request cache must not outlive the freed declaring op_array
--INI--
opcache.enable_cli=0
--FILE--
<?php
function f($a = 'A', $b = 'B', $c = 'C', $d = 'D') { return "$a|$b|$c|$d"; }

$file = __DIR__ . '/cache_freed_declaring_site_005.inc';
for ($round = 0; $round < 2; $round++) {
    for ($argc = 1; $argc <= 4; $argc++) {
        for ($mask = 1; $mask < (1 << $argc); $mask++) {
            $args = [];
            $expected = ['A', 'B', 'C', 'D'];
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
            file_put_contents($file, "<?php return $shape;");
            $pfa = include $file;
            $result = $pfa(...range(1, $placeholders));
            if ($result !== implode('|', $expected)) {
                echo "$shape: got $result\n";
            }
        }
    }
}
echo "Done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/cache_freed_declaring_site_005.inc');
?>
--EXPECT--
Done
