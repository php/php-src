--TEST--
GHSA-j3wh-g957-2m85 (tar entry injection via invalid entry size)
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

function tar_dump(string $fname): void
{
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

$base = __DIR__ . '/' . basename(__FILE__, '.php');

/* the entry after the carrier is never reached by a conforming tar reader */
$sizes = [
    'octal overflow to 0'    => "40000000000\0",
    'octal overflow to 1'    => "40000000001\0",
    'octal padding overflow' => "37777777400\0",
    'GNU base-256'           => "\x80" . str_repeat("\0", 9) . "\x02\x00",
    'non-octal digits'       => "99999999999\0",
    'trailing garbage'       => "0000000001XX",
];

$i = 0;
foreach ($sizes as $label => $size) {
    $fname = $base . '.' . $i++ . '.tar';
    $tar  = tar_header('normal.txt', sprintf("%011o\0", 9)) . str_pad('NORMAL_OK', 512, "\0");
    $tar .= tar_header('carrier.bin', $size);
    $tar .= tar_header('injected.txt', sprintf("%011o\0", 8)) . str_pad('INJECTED', 512, "\0");
    $tar .= str_repeat("\0", 1024);
    file_put_contents($fname, $tar);

    echo $label, ': ';
    tar_dump($fname);
}

$fname = $base . '.valid.tar';
$tar  = tar_header('normal.txt', sprintf("%011o\0", 9)) . str_pad('NORMAL_OK', 512, "\0");
$tar .= tar_header('second.txt', sprintf("%011o\0", 8)) . str_pad('SECOND__', 512, "\0");
$tar .= str_repeat("\0", 1024);
file_put_contents($fname, $tar);

echo 'valid archive: ';
tar_dump($fname);
?>
--CLEAN--
<?php
foreach (glob(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.*.tar') as $file) {
    unlink($file);
}
?>
--EXPECTF--
octal overflow to 0: phar error: "%s" is a corrupted tar file (invalid entry size)
octal overflow to 1: phar error: "%s" is a corrupted tar file (invalid entry size)
octal padding overflow: phar error: "%s" is a corrupted tar file (invalid entry size)
GNU base-256: phar error: "%s" is a corrupted tar file (invalid entry size)
non-octal digits: phar error: "%s" is a corrupted tar file (invalid entry size)
trailing garbage: phar error: "%s" is a corrupted tar file (invalid entry size)
valid archive: normal.txt, second.txt
