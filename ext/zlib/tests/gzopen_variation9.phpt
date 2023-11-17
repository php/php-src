--TEST--
Test gzopen() function : variation: try opening with possibly invalid modes
--EXTENSIONS--
zlib
--FILE--
<?php
echo "*** Testing gzopen() : variation ***\n";

$modes = array('r+', 'rf', 'w+' , 'e');

$file = __DIR__."/004.txt.gz";

foreach ($modes as $mode) {
    echo "mode=$mode\n";
    $h = false;
    try {
        $h = gzopen($file, $mode);
    } catch (\Throwable $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    echo "gzopen=";
    var_dump($h);
    if ($h !== false) {
       gzclose($h);
    }
    echo "\n";
}
?>
--EXPECTF--
*** Testing gzopen() : variation ***
mode=r+
Cannot open a zlib stream for reading and writing at the same time!
gzopen=bool(false)

mode=rf
gzopen=resource(%d) of type (stream)

mode=w+
Cannot open a zlib stream for reading and writing at the same time!
gzopen=bool(false)

mode=e

Warning: gzopen(%s/004.txt.gz): Failed to open stream: %s in %s on line %d
gzopen=bool(false)

