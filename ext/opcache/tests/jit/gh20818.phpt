--TEST--
GH-20818 (Segfault in Tracing JIT with Object Reference)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=1M
--FILE--
<?php

function process($data) {
    foreach ($data as &$v) {}
}

$data = [
    (object) ["" => 1],
    (object) ["" => 1],
    (object) [],
];

for ($i = 0; $i < 200; $i += 1) {
    foreach ($data as $entry) {
        process($entry);
    }
}

echo "Done\n";
?>
--EXPECT--
Done
