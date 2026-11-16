--TEST--
GH-25480: Tracing JIT does not reuse stale FETCH_DIM_IS result
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_buffer_size=16M
--EXTENSIONS--
opcache
--FILE--
<?php
class Row {
    public function __get(string $name): int {
        return 1;
    }
}

function take(?array $a): bool {
    return $a === [] || $a === ['present'];
}

function run(array $rows, array $map, ?object $none): int {
    $bad = 0;
    foreach ($rows as $id => $row) {
        if ($none?->id == $id) {}
        $x = $row->a && $row->b;
        try {
            if (!take($map[$id] ?? [])) {
                $bad++;
            }
        } catch (TypeError $e) {
            $bad++;
        }
    }
    return $bad;
}

$rows = array_fill(0, 100, new Row());
$bad = 0;
for ($i = 0; $i < 10; $i++) {
    $bad += run($rows, [0 => ['present']], null);
}
echo "Bad values: $bad\n";
?>
--EXPECT--
Bad values: 0
