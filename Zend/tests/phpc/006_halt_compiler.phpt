--TEST--
.phpc: __halt_compiler() works and __COMPILER_HALT_OFFSET__ points to trailing data
--FILE--
<?php
$file = __DIR__ . '/' . basename(__FILE__, '.php') . '.phpc';
/* The .phpc file echoes a marker, halts the compiler, and the driver here
 * reads what comes after __halt_compiler() back via the offset constant
 * (which __halt_compiler() exposes in the loaded file's namespace). */
$code = <<<'PHPC'
$offset = __COMPILER_HALT_OFFSET__;
$fh = fopen(__FILE__, 'rb');
fseek($fh, $offset);
$trail = trim(fread($fh, 8192));
fclose($fh);
echo "before-halt\n", $trail, "\n";
__halt_compiler();
TRAILING-DATA-12345
PHPC;
file_put_contents($file, $code);
register_shutdown_function(fn() => @unlink($file));
require $file;
?>
--EXPECT--
before-halt
TRAILING-DATA-12345
