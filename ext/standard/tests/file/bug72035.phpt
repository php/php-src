--TEST--
Bug #72035 php-cgi.exe fails to run scripts relative to drive root
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' ) die('skip windows only test');
if(php_sapi_name() != "cli") die('skip CLI only test');

$cgi = realpath(dirname(PHP_BINARY)) . DIRECTORY_SEPARATOR . "php-cgi.exe";
if (!file_exists($cgi)) die('skip CGI binary not found');
?>
--FILE--
<?php

$fl = dirname(__FILE__) . DIRECTORY_SEPARATOR . md5(uniqid()) . ".php";
$fl = substr($fl, 2);

$cgi = realpath(dirname(PHP_BINARY) . DIRECTORY_SEPARATOR . "php-cgi.exe");

file_put_contents($fl, "<?php echo \"hello\", \"\n\"; ?>");

$cmd = "$cgi -n -C $fl";

/* Need to run CGI with the env reset. */
$desc = array(0 => array("pipe", "r"));
$proc = proc_open($cmd, $desc, $pipes, getcwd(), array());
if (is_resource($proc)) {
	echo stream_get_contents($pipes[0]);	

	proc_close($proc);
}

unlink($fl);
?>
==DONE==
--EXPECTF--
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

hello
==DONE==
