--TEST--
Test fopen() function : variation: use include path and stream context relative/absolute file
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
echo "*** Testing fopen() : variation ***\n";
$absfile = __FILE__.'.tmp';
$relfile = "fopen_variation6.tmp";

$h = fopen($absfile, "w");
fwrite($h, "This is an absolute file");
fclose($h);

$h = fopen($relfile, "w");
fwrite($h, "This is a relative file");
fclose($h);

$ctx = stream_context_create();
$h = fopen($absfile, "r", true, $ctx);
fpassthru($h);
fclose($h);
echo "\n";

$h = fopen($relfile, "r", true, $ctx);
fpassthru($h);
fclose($h);
echo "\n";

unlink($absfile);
unlink($relfile);
?>
--EXPECT--
*** Testing fopen() : variation ***
This is an absolute file
This is a relative file
