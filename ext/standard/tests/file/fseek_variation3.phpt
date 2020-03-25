--TEST--
Test fseek() function : variation functionality beyond file boundaries
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : proto int fseek(resource fp, int offset [, int whence])
 * Description: Seek on a file pointer
 * Source code: ext/standard/file.c
 * Alias to functions: gzseek
 */

echo "*** Testing fseek() : variation - beyond file boundaries ***\n";

$outputfile = __FILE__.".tmp";

$h = fopen($outputfile, "wb+");
for ($i = 1; $i < 10; $i++) {
   fwrite($h, chr(0x30 + $i));
}

echo "--- fseek beyond start of file ---\n";
var_dump(fseek($h, -4, SEEK_SET));
echo "after -4 seek: ".bin2hex(fread($h,1))."\n";
var_dump(fseek($h, -1, SEEK_CUR));
echo "after seek back 1: ".bin2hex(fread($h,1))."\n";
var_dump(fseek($h, -20, SEEK_CUR));
echo "after seek back 20: ".bin2hex(fread($h,1))."\n";

echo "--- fseek beyond end of file ---\n";
var_dump(fseek($h, 16, SEEK_SET));
fwrite($h, "end");
fseek($h ,0, SEEK_SET);
$data = fread($h, 4096);
echo bin2hex($data)."\n";

fclose($h);
unlink($outputfile);

echo "Done";
?>
--EXPECT--
*** Testing fseek() : variation - beyond file boundaries ***
--- fseek beyond start of file ---
int(-1)
after -4 seek: 
int(0)
after seek back 1: 39
int(-1)
after seek back 20: 
--- fseek beyond end of file ---
int(0)
31323334353637383900000000000000656e64
Done
