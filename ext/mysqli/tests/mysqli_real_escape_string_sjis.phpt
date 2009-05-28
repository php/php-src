--TEST--
mysqli_real_escape_string() - sjis
--SKIPIF--
<?php
if (version_compare(PHP_VERSION, '5.9.9', '>') == 1) {
	die('skip set character set not functional with PHP 6 (fomerly PHP 6 && unicode.semantics=On)');
}

require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');

if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
	die(sprintf("skip Cannot connect to MySQL, [%d] %s\n",
		mysqli_connect_errno(), mysqli_connect_error()));
}
if (!mysqli_set_charset($link, 'sjis'))
	die(sprintf("skip Cannot set charset 'sjis'"));
mysqli_close($link);
?>
--FILE--
<?php

	require_once("connect.inc");
	require_once('table.inc');

	var_dump(mysqli_set_charset($link, "sjis"));

	if ('?p??\\\\?p??' !== ($tmp = mysqli_real_escape_string($link, '?p??\\?p??')))
		printf("[004] Expecting \\\\, got %s\n", $tmp);

	if ('?p??\"?p??' !== ($tmp = mysqli_real_escape_string($link, '?p??"?p??')))
		printf("[005] Expecting \", got %s\n", $tmp);

	if ("?p??\'?p??" !== ($tmp = mysqli_real_escape_string($link, "?p??'?p??")))
		printf("[006] Expecting ', got %s\n", $tmp);

	if ("?p??\\n?p??" !== ($tmp = mysqli_real_escape_string($link, "?p??\n?p??")))
		printf("[007] Expecting \\n, got %s\n", $tmp);

	if ("?p??\\r?p??" !== ($tmp = mysqli_real_escape_string($link, "?p??\r?p??")))
		printf("[008] Expecting \\r, got %s\n", $tmp);

	if ("?p??\\0?p??" !== ($tmp = mysqli_real_escape_string($link, "?p??" . chr(0) . "?p??")))
		printf("[009] Expecting %s, got %s\n", "?p??\\0?p??", $tmp);

	var_dump(mysqli_query($link, "INSERT INTO test(id, label) VALUES (100, '?p')"));

	mysqli_close($link);
	print "done!";
?>
--EXPECTF--
bool(true)
bool(true)
done!
