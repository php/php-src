--TEST--
GHSA-j3wh-g957-2m85 (tar entry injection via entry types that carry data)
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
function tar_header(string $name, string $size, string $typeflag = '0'): string
{
    $header = str_repeat("\0", 512);
    $header = substr_replace($header, $name, 0, strlen($name));
    $header = substr_replace($header, sprintf("%07o\0", 0644), 100, 8);
    $header = substr_replace($header, sprintf("%07o\0", 0), 108, 8);
    $header = substr_replace($header, sprintf("%07o\0", 0), 116, 8);
    $header = substr_replace($header, $size, 124, 12);
    $header = substr_replace($header, sprintf("%011o\0", 1700000000), 136, 12);
    $header[156] = $typeflag;
    $header = substr_replace($header, "ustar\0", 257, 6);
    $header = substr_replace($header, "00", 263, 2);

    $checksum = 0;
    for ($i = 0; $i < 512; $i++) {
        $checksum += ($i >= 148 && $i < 156) ? 0x20 : ord($header[$i]);
    }

    return substr_replace($header, sprintf("%06o\0 ", $checksum), 148, 8);
}

$base = __DIR__ . '/' . basename(__FILE__, '.php');

/* '5' and the other types below carry no data in any tar reader, so the
 * injected header stays visible to all of them and is not hidden from
 * anything that inspects the archive with a different implementation.
 * 'K' is a GNU metadata record that is not supported and is refused. */
$typeflags = [
    'contiguous file' => '7',
    'unknown type'    => 'Z',
    'GNU long link'   => 'K',
    'directory'       => '5',
    'fifo'            => '6',
];

$i = 0;
foreach ($typeflags as $label => $typeflag) {
    $fname = $base . '.' . $i++ . '.tar';
    $tar  = tar_header('normal.txt', sprintf("%011o\0", 9)) . str_pad('NORMAL_OK', 512, "\0");
    $tar .= tar_header('carrier.bin', sprintf("%011o\0", 512), $typeflag);
    /* the carrier's only data block is a tar header a conforming reader skips */
    $tar .= tar_header('injected.txt', sprintf("%011o\0", 0));
    $tar .= str_repeat("\0", 1024);
    file_put_contents($fname, $tar);

    echo $label, ': ';
    try {
        $phar = new PharData($fname);
        $names = [];
        foreach (new RecursiveIteratorIterator($phar) as $file) {
            $names[] = $file->getFilename();
        }
        sort($names);
        echo implode(', ', $names), "\n";
    } catch (UnexpectedValueException $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--CLEAN--
<?php
foreach (glob(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.*.tar') as $file) {
    unlink($file);
}
?>
--EXPECTF--
contiguous file: carrier.bin, normal.txt
unknown type: carrier.bin, normal.txt
GNU long link: phar error: "%s" is a tar file with an unsupported GNU long link entry
directory: injected.txt, normal.txt
fifo: carrier.bin, injected.txt, normal.txt
