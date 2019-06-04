--TEST--
mb_str_begins()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
// TODO: Add more encodings
ini_set('include_path','.');
include_once('common.inc');

// Test string
$euc_jp = '0123この文字列は日本語です。EUC-JPを使っています。0123日本語は面倒臭い。';

// EUC-JP - With encoding parameter
mb_internal_encoding('UTF-8') or print("mb_internal_encoding() failed\n");
var_dump(mb_str_begins($euc_jp, '0123', 'EUC-JP'));
var_dump(mb_str_begins($euc_jp, '韓国語', 'EUC-JP'));

// EUC-JP - No encoding parameter
mb_internal_encoding('EUC-JP')  or print("mb_internal_encoding() failed\n");
var_dump(mb_str_begins($euc_jp, '0123'));
var_dump(mb_str_begins($euc_jp, '韓国語'));

// Basic functionality
mb_internal_encoding('UTF-8');
$string_ascii = 'abc def';

//Japanese string in UTF-8
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

var_dump(mb_str_begins($string_ascii, 'a', 'ISO-8859-1'));
var_dump(mb_str_begins($string_ascii, 'b', 'ISO-8859-1'));
var_dump(mb_str_begins($string_ascii, 'abc'));
var_dump(mb_str_begins($string_ascii, 'bc'));

$needle1 = base64_decode('5pel5pys6Kqe');
var_dump(mb_str_begins($string_mb, $needle1));

$needle2 = base64_decode("44GT44KT44Gr44Gh44Gv44CB5LiW55WM");
var_dump(mb_str_begin($string_mb, $needle2));


?>
==DONE==
--EXPECTF--
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
