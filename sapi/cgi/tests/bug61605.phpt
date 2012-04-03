--TEST--
header_remove() should handle multiple same key headers
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

test('<?php
header("X-Foo: Bar");
header("X-Foo: Bar2", false);

header_remove("X-Foo");
?>');

test('<?php
header("X-Foo: Bar");
header("X-Foo: Baz", false);
header("X-Foo: Baz2");
?>');

@unlink($f);
?>
--EXPECTF--
----------
<?php
header("X-Foo: Bar");
header("X-Foo: Bar2", false);

header_remove("X-Foo");
?>
----------
X-Powered-By: PHP/%s
Content-type: text/html

----------
<?php
header("X-Foo: Bar");
header("X-Foo: Baz", false);
header("X-Foo: Baz2");
?>
----------
X-Powered-By: PHP/%s
X-Foo: Baz2
Content-type: text/html
