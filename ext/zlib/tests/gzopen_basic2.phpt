--TEST--
Test gzopen() function : basic functionality for writing
--EXTENSIONS--
zlib
--FILE--
<?php
echo "*** Testing gzopen() : basic functionality ***\n";


// Initialise all required variables
$filename = "gzopen_basic2.txt.gz";
$modes = array('w', 'w+');
$data = "This was the information that was written";

foreach($modes as $mode) {
    echo "testing mode -- $mode --\n";
    $h = false;
    try {
        $h = gzopen($filename, $mode);
    } catch (\Throwable $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    if ($h !== false) {
        gzwrite($h, $data);
        gzclose($h);
        $h = gzopen($filename, 'r');
        gzpassthru($h);
        gzclose($h);
        echo "\n";
        unlink($filename);
    }
    else {
        var_dump($h);
    }
}

?>
--EXPECTF--
*** Testing gzopen() : basic functionality ***
testing mode -- w --
This was the information that was written
testing mode -- w+ --
Cannot open a zlib stream for reading and writing at the same time!
bool(false)
