--TEST--
header_remove()
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

$f = tempnam(sys_get_temp_dir(), 'cgitest');

function test($script) {
	file_put_contents($GLOBALS['f'], $script);
	$cmd = escapeshellcmd($GLOBALS['php']);
	$cmd .= ' -n -dreport_zend_debug=0 -dhtml_errors=0 ' . escapeshellarg($GLOBALS['f']);
	echo "----------\n";
	echo rtrim($script) . "\n";
	echo "----------\n";
	passthru($cmd);
}

test('<?php ?>');
test('<?php header_remove(); ?>');
test('<?php header_remove("X-Foo"); ?>');
test('<?php
header("X-Foo: Bar");
?>');
test('<?php
header("X-Foo: Bar");
header("X-Bar: Baz");
header_remove("X-Foo");
?>');
test('<?php
header("X-Foo: Bar");
header_remove("X-Foo: Bar");
?>');
test('<?php
header("X-Foo: Bar");
header_remove("X-Foo:");
?>');
test('<?php
header("X-Foo: Bar");
header_remove();
?>');
test('<?php
header_remove("");
?>');
test('<?php
header_remove(":");
?>');
test('<?php
header("X-Foo: Bar");
echo "flush\n";
flush();
header_remove("X-Foo");
?>');

@unlink($f);
?>
--EXPECTF--
----------
<?php ?>
----------
X-Powered-By: PHP/%s
Content-type: text/html

----------
<?php header_remove(); ?>
----------
Content-type: text/html

----------
<?php header_remove("X-Foo"); ?>
----------
X-Powered-By: PHP/%s
Content-type: text/html

----------
<?php
header("X-Foo: Bar");
?>
----------
X-Powered-By: PHP/%s
X-Foo: Bar
Content-type: text/html

----------
<?php
header("X-Foo: Bar");
header("X-Bar: Baz");
header_remove("X-Foo");
?>
----------
X-Powered-By: PHP/%s
X-Bar: Baz
Content-type: text/html

----------
<?php
header("X-Foo: Bar");
header_remove("X-Foo: Bar");
?>
----------
X-Powered-By: PHP/%s
X-Foo: Bar
Content-type: text/html


Warning: Header to delete may not contain colon. in %s on line 3
----------
<?php
header("X-Foo: Bar");
header_remove("X-Foo:");
?>
----------
X-Powered-By: PHP/%s
X-Foo: Bar
Content-type: text/html


Warning: Header to delete may not contain colon. in %s on line 3
----------
<?php
header("X-Foo: Bar");
header_remove();
?>
----------
Content-type: text/html

----------
<?php
header_remove("");
?>
----------
X-Powered-By: PHP/%s
Content-type: text/html

----------
<?php
header_remove(":");
?>
----------
X-Powered-By: PHP/%s
Content-type: text/html


Warning: Header to delete may not contain colon. in %s on line 2
----------
<?php
header("X-Foo: Bar");
echo "flush\n";
flush();
header_remove("X-Foo");
?>
----------
X-Powered-By: PHP/%s
X-Foo: Bar
Content-type: text/html

flush

Warning: Cannot modify header information - headers already sent by (output started at %s:3) in %s on line 5
