--TEST--
bzopen() on a stream resource: the inner stream may be closed first
--EXTENSIONS--
bz2
--FILE--
<?php
$file = tempnam(sys_get_temp_dir(), 'bz2');
$victim = tempnam(sys_get_temp_dir(), 'bz2');
$payload = str_repeat("payload", 100);

$fp = fopen($file, 'w');
$bz = bzopen($fp, 'w');
var_dump(bzwrite($bz, $payload));

// The bz2 stream owns its own descriptor, so closing the inner stream neither
// invalidates it nor makes it write into the next descriptor opened
var_dump(fclose($fp));
$other = fopen($victim, 'w');
bzclose($bz);
var_dump(fclose($other));

var_dump(bzdecompress(file_get_contents($file)) === $payload);
var_dump(filesize($victim));

$fp = fopen($file, 'r');
$bz = bzopen($fp, 'r');
var_dump(fclose($fp));
var_dump(bzread($bz, 8192) === $payload);
bzclose($bz);

unlink($file);
unlink($victim);
?>
--EXPECT--
int(700)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
