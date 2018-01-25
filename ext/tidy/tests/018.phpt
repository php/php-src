--TEST--
binary safety
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php
$x = tidy_repair_string("<p>abra\0cadabra</p>",
	array(	'show-body-only' => true,
			'clean' => false,
			'newline' => "\n")
	);
var_dump($x);
?>
--EXPECT--
string(19) "<p>abracadabra</p>
"
