--TEST--
Bug #40189 (endless loop in zlib.inflate stream filter)
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
// this string is an excerpt of a phar archive that caused an infinite loop
$a = "\x3\x0\x85\x46\x2f\x7c\xc2\xaa\x69\x2b\x6d\xe5\xdb\xfe\xe4\x21\x8f\x0\x97\x21\x1d\x2\x0\x0\x0\x47\x42\x4d\x42";
var_dump(base64_encode($a));
$gp = fopen(__DIR__ . '/test.other', 'wb');
$fp = fopen('data://text/plain;base64,AwCFRi98wqppK23l2/7kIY8AlyEdAgAAAEdCTUI=', 'r');
stream_filter_append($fp, 'zlib.inflate', STREAM_FILTER_READ);
var_dump(stream_copy_to_stream($fp, $gp, 5));
fclose($fp);
fclose($gp);
var_dump(file_get_contents(__DIR__ . '/test.other'));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/test.other');
?>
--EXPECT--
string(40) "AwCFRi98wqppK23l2/7kIY8AlyEdAgAAAEdCTUI="
int(0)
string(0) ""
