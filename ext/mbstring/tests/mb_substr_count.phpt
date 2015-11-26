--TEST--
mb_substr_count()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
--FILE--
<?php
	mb_internal_encoding("EUC-JP");
	var_dump(@mb_substr_count("", ""));
	var_dump(@mb_substr_count("あ", ""));
	var_dump(@mb_substr_count("", "あ"));
	var_dump(@mb_substr_count("", "あ"));
	var_dump(@mb_substr_count("", chr(0)));

	$a = str_repeat("abcacba", 100);
	var_dump(@mb_substr_count($a, "bca"));

	$a = str_repeat("あいうあういあ", 100);
	$b = "いうあ";
	var_dump(@mb_substr_count($a, $b));

	$to_enc = "UTF-8";
	var_dump(@mb_substr_count(mb_convert_encoding($a, $to_enc),
	                          mb_convert_encoding($b, $to_enc), $to_enc));

	$to_enc = "Shift_JIS";
	var_dump(@mb_substr_count(mb_convert_encoding($a, $to_enc),
	                          mb_convert_encoding($b, $to_enc), $to_enc));

	$a = str_repeat("abcacbabca", 100);
	var_dump(@mb_substr_count($a, "bca"));
?>
--EXPECT--
bool(false)
bool(false)
int(0)
int(0)
int(0)
int(100)
int(100)
int(100)
int(100)
int(200)
