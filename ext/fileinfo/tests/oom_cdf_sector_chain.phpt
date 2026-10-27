--TEST--
fileinfo on cdf file doesn't reach hard limit
--EXTENSIONS--
fileinfo
--FILE--
<?php
/*
 * The header advertises 32 KiB sectors and two FAT sectors.  The FAT contains
 * a 10,000-entry mini-FAT chain even though the file has only three sectors.
 * cdf_read_ssat() consequently attempts to allocate 10,000 * 32 KiB before
 * validating that the chain data is present in the input.
 */
$sectorSize = 32 * 1024;
$header =
    "\xD0\xCF\x11\xE0\xA1\xB1\x1A\xE1" . // CDF signature
    str_repeat("\0", 16) .                 // CLSID
    pack('vvvvv', 0x3e, 4, 0xfffe, 15, 6) .
    str_repeat("\0", 10) .
    pack('V8',
        2,          // Number of FAT sectors
        2,          // First directory sector
        0,          // Transaction signature
        4096,       // Mini-stream cutoff
        2,          // First mini-FAT sector
        10000,      // Number of mini-FAT sectors
        -2,         // End of DIFAT chain
        0,          // Number of DIFAT sectors
    ) .
    pack('V2', 0, 1) .
    str_repeat(pack('V', -1), 107);
assert(strlen($header) === 512);

$fat = array_fill(0, 16384, -1);
$fat[0] = -3;
$fat[1] = -3;
for ($i = 2; $i < 10001; $i++) {
    $fat[$i] = $i + 1;
}
$fat[10001] = -2;

$path = __DIR__ . '/oom_cdf_sector_chain.cdf';
$file = fopen($path, 'wb');
fwrite($file, $header);
fwrite($file, str_repeat("\0", $sectorSize - strlen($header)));
foreach ($fat as $sector) {
    fwrite($file, pack('V', $sector));
}
fwrite($file, str_repeat("\0", $sectorSize));
fclose($file);

function run(string $code): void
{
    $command = escapeshellarg(getenv('TEST_PHP_EXECUTABLE')) . ' ' .  getenv('TEST_PHP_EXTRA_ARGS') .  ' -d memory_limit=128M -d html_errors=0 -r ' .  escapeshellarg($code);
    $process = proc_open($command, [STDIN, STDOUT, STDERR], $pipes);
    $status = proc_close($process);
    echo "exit_status=$status\n";
}

$exportedPath = var_export($path, true);
run('$f = finfo_open(FILEINFO_MIME_TYPE); finfo_file($f, ' . $exportedPath . ');');
run('$f = finfo_open(FILEINFO_MIME_TYPE); finfo_buffer($f, file_get_contents(' . $exportedPath . '));');
run('mime_content_type(' . $exportedPath . ');');
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_cdf_sector_chain.cdf');
?>
--EXPECTF--
Fatal error: Allowed memory size of 134217728 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
exit_status=255

Fatal error: Allowed memory size of 134217728 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
exit_status=255

Fatal error: Allowed memory size of 134217728 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
exit_status=255
