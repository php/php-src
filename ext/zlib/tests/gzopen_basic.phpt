--TEST--
Test gzopen() function : basic functionality
--EXTENSIONS--
zlib
--FILE--
<?php
echo "*** Testing gzopen() : basic functionality ***\n";


// Initialise all required variables
$filename = __DIR__."/004.txt.gz";
$mode = 'r';
$use_include_path = false;

// Calling gzopen() with all possible arguments
$h = gzopen($filename, $mode, $use_include_path);
gzpassthru($h);
gzclose($h);

// Calling gzopen() with mandatory arguments
$h = gzopen($filename, $mode);
gzpassthru($h);
gzclose($h);

?>
--EXPECT--
*** Testing gzopen() : basic functionality ***
When you're taught through feelings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns around
and I know that it descends down on me
When you're taught through feelings
Destiny flying high above
all I know is that you can realize it
Destiny who cares
as it turns around
and I know that it descends down on me
