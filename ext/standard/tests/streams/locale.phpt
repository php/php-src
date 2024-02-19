--TEST--
Stream wrappers should not be locale dependent
--SKIPIF--
<?php  // try to activate a single-byte german locale
if (!setlocale(LC_ALL, 'de_DE','de_DE.ISO8859-1','de_DE.ISO_8859-1')) {
    print "skip Can't find german locale\n";
}
?>
--INI--
allow_url_fopen=1
display_errors=stderr
--FILE--
<?php
setlocale(LC_ALL, 'de_DE', 'de_DE.ISO8859-1', 'de_DE.ISO_8859-1');
class testwrapper {
}

var_dump(stream_wrapper_register("test٪", 'testwrapper', STREAM_IS_URL));

echo 'stream_open: ';
fopen("test٪://test", 'r');
?>
--EXPECTF--
Warning: stream_wrapper_register(): Invalid protocol scheme specified. Unable to register wrapper class testwrapper to test٪:// in %s on line 6
bool(false)
stream_open: Warning: fopen(test٪://test): Failed to open stream: No such file or directory in %s on line 9
