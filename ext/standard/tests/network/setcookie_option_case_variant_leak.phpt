--TEST--
setcookie() does not leak when an option array has case-variant duplicate keys
--FILE--
<?php
$base = memory_get_usage();
for ($i = 0; $i < 5000; $i++) {
    @setcookie('name', 'value', ['path' => '/aaaaaaaaaaaaaaaa' . $i, 'Path' => '/bbbbbbbbbbbbbbbb' . $i]);
    header_remove();
}
// Each duplicate-key call leaked the first path string before the fix,
// growing usage by tens of bytes per iteration (hundreds of KB here).
var_dump(memory_get_usage() - $base < 50000);
?>
--EXPECT--
bool(true)
