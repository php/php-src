--TEST--
Bug #81659 (stream_get_contents() may unnecessarily overallocate)
--FILE--
<?php
$stream = fopen(__DIR__ . "/81659.txt", "w+");

for ($i = 0; $i < 1024; $i++) {
    fwrite($stream, str_repeat("*", 1024));
}

fseek($stream, 1023 * 1024);

$m0 = memory_get_peak_usage();
var_dump(strlen(stream_get_contents($stream)));
$m1 = memory_get_peak_usage();
var_dump($m1 < $m0 + 512 * 1024);
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/81659.txt");
?>
--EXPECT--
int(1024)
bool(true)
