--TEST--
stream_select(): >64 non-socket handles fail gracefully on Windows (no stack overflow)
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
?>
--FILE--
<?php
/* On Windows stream_select() multiplexes non-socket handles (pipes, files)
 * through WaitForMultipleObjects(), which cannot wait on more than
 * MAXIMUM_WAIT_OBJECTS (64) handles at once. php_select() used to push each
 * handle into a fixed 64-slot stack array with no bounds check, so selecting on
 * 65+ file/pipe handles overran the stack and crashed the process with
 * STATUS_STACK_BUFFER_OVERRUN (0xC0000409). It must instead fail gracefully
 * (warning + false). This test crashes every unpatched PHP build on Windows and
 * passes only once the cap is in place. */
$files = [];
for ($i = 0; $i < 70; $i++) {
    $path = __DIR__ . "/stream_select_win32_max_handles_$i.tmp";
    file_put_contents($path, "d");
    $files[] = fopen($path, "rb");
}

$r = $files;
$w = $e = null;
var_dump(stream_select($r, $w, $e, 0, 0));

foreach ($files as $f) {
    fclose($f);
}
?>
--EXPECTF--
Warning: stream_select(): Unable to select [%d]: %s (max_fd=%d) in %s on line %d
bool(false)
--CLEAN--
<?php
for ($i = 0; $i < 70; $i++) {
    @unlink(__DIR__ . "/stream_select_win32_max_handles_$i.tmp");
}
?>
