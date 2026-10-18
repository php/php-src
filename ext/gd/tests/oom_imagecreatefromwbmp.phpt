--TEST--
imagecreatefromwbmp() doesn't reach hard limit
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) die("skip requires bundled GD library");
if (!function_exists("imagecreatefromwbmp")) die("skip WBMP support unavailable");
?>
--INI--
memory_limit=64M
--FILE--
<?php
$mbi = function (int $v): string {          // WBMP multi-byte integer
    $out = chr($v & 0x7f);
    for ($v >>= 7; $v > 0; $v >>= 7) {
        $out = chr(0x80 | ($v & 0x7f)) . $out;
    }
    return $out;
};
// 16384 x 16384 x sizeof(int) = 1 GB
$wbmp = "\x00\x00" . $mbi(16384) . $mbi(16384) . str_repeat("\x00", 64);

$path = __DIR__ . '/oom_imagecreatefromwbmp.tmp.wbmp';
file_put_contents($path, $wbmp);
imagecreatefromwbmp($path);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_imagecreatefromwbmp.tmp.wbmp');
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
