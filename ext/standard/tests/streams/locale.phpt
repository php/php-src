--TEST--
Stream wrappers should not be locale dependent
--SKIPIF--
<?php  // try to activate a single-byte german locale
if (!setlocale(LC_ALL, "de_DE")) {
    print "skip Can't find german locale";
}
?>
--INI--
allow_url_fopen=1
display_errors=stderr
--FILE--
<?php
setlocale(LC_ALL, "de_DE");
class testwrapper {
}

var_dump(ctype_alpha('٪'));  // \xd9 and \xaa are both alphabetical in the german locale
var_dump(stream_wrapper_register("test٪", 'testwrapper', STREAM_IS_URL));

echo 'stream_open: ';
fopen("test٪://test", 'r');
?>
--EXPECTF--
bool(true)
Warning: stream_wrapper_register(): Invalid protocol scheme specified. Unable to register wrapper class testwrapper to test٪:// in %s on line 7
bool(false)
stream_open: Warning: fopen(test٪://test): Failed to open stream: No such file or directory in %s on line 10
