--TEST--
gzfile(): NUL bytes must be preserved and long lines must not be split
--EXTENSIONS--
zlib
--FILE--
<?php
$tmp = tempnam(sys_get_temp_dir(), 'gzfile');

// --- NUL byte preservation ---
$data_nul = "avant\0apres\nligne2\n";
file_put_contents($tmp, gzencode($data_nul));

$lines = gzfile($tmp);
var_dump(count($lines));                    // 2
var_dump(strlen($lines[0]));                // 12 (avant\0apres\n)
var_dump(bin2hex($lines[0]));               // contains NUL

// Matches gzgets line-by-line
$h = gzopen($tmp, 'rb');
$gzgets_lines = [];
while (($l = gzgets($h)) !== false) { $gzgets_lines[] = $l; }
gzclose($h);
var_dump($lines === $gzgets_lines);         // true

// --- Long line: no splitting ---
$long_line = str_repeat('x', 20000) . "\n";
file_put_contents($tmp, gzencode($long_line));

$lines = gzfile($tmp);
var_dump(count($lines));                    // 1
var_dump(strlen($lines[0]));                // 20001

unlink($tmp);
echo "Done\n";
?>
--EXPECT--
int(2)
int(12)
string(24) "6176616e740061707265730a"
bool(true)
int(1)
int(20001)
Done
