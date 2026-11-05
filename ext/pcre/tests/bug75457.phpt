--TEST--
Bug #75457 (heap-use-after-free in php7.0.25)
--SKIPIF--
<?php
if (PCRE_VERSION_MAJOR >= 10 && PCRE_VERSION_MINOR >= 45) {
	die("skip callout feature is disable in PCRE2 10.45 and above");
}
?>
--FILE--
<?php
$pattern = "/(((?(?C)0?=))(?!()0|.(?0)0)())/";
var_dump(preg_match($pattern, "hello"));
?>
--EXPECTF--
Warning: preg_match(): Compilation failed:%r( atomic|)%r assertion expected after (?( or (?(?C) at offset 8 in %sbug75457.php on line %d
bool(false)
