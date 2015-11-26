--TEST--
fread & fwrite - Test reading and writing using a single resource
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php

/* 
 * proto int fwrite(resource fp, string str [, int length])
 * Function is implemented in ext/standard/file.c
 */ 
 
 /*
 Prototype: string fread ( resource $handle [, int $length] );
 Description: reads up to length bytes from the file pointer referenced by handle. 
   Reading stops when up to length bytes have been read, EOF (end of file) is 
   reached, (for network streams) when a packet becomes available, or (after 
   opening userspace stream) when 8192 bytes have been read whichever comes first.
*/


$outputfile = __FILE__.".tmp";

echo "--- testing rw moving about the file ---\n";
$h = fopen($outputfile, 'wb+');
$out1 = "The 1st prrt";
$out2 = " part of the ttxt";
$out3 = "text";
fwrite($h, $out1);
fseek($h, 0, SEEK_SET);
echo "start:".fread($h, strlen($out1) - 5). "\n";
fwrite($h, $out2);
echo "at end:".fread($h,100)."\n";
var_dump(feof($h));
fseek($h, -4, SEEK_CUR);
fwrite($h, $out3);
fseek($h, 0, SEEK_SET);
echo "final:".fread($h, 100)."\n";
fclose($h);

echo "--- testing eof ---\n";
$h = fopen($outputfile, 'ab+');
fread($h,1024);
var_dump(feof($h));
fread($h,1);
var_dump(feof($h));
$out = "extra";
fwrite($h, $out);
var_dump(feof($h));
fread($h,1);
var_dump(feof($h));
fseek($h, -strlen($out) + 1, SEEK_CUR);
echo "last bytes: ".fread($h, strlen($out))."\n";
fclose($h);

unlink($outputfile);

echo "Done";
?>
--EXPECT--
--- testing rw moving about the file ---
start:The 1st
at end:
bool(true)
final:The 1st part of the text
--- testing eof ---
bool(true)
bool(true)
bool(true)
bool(true)
last bytes: xtra
Done