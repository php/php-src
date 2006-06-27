--TEST--
libxml_set_streams_context()
--SKIPIF--
<?php if (!extension_loaded('dom')) die('skip'); ?>
--FILE--
<?php

$ctxs = array(
	NULL,
	'bogus',
	123,
	new stdclass,
	array('a'),
	stream_context_create(),
	stream_context_create(array('file')),
	stream_context_create(array('file' => array('some_opt' => 'aaa')))
);


foreach ($ctxs as $ctx) {
	var_dump(libxml_set_streams_context($ctx));
	$dom = new DOMDocument();
	var_dump($dom->load(dirname(__FILE__).'/test.xml'));
}

echo "Done\n";

?>
--EXPECTF--
Warning: options should have the form ["wrappername"]["optionname"] = $value in %s004.php on line 10
NULL

Warning: DOMDocument::load(): supplied argument is not a valid Stream-Context resource in %s004.php on line 18
bool(true)
NULL

Warning: DOMDocument::load(): supplied argument is not a valid Stream-Context resource in %s004.php on line 18
bool(true)
NULL

Warning: DOMDocument::load(): supplied argument is not a valid Stream-Context resource in %s004.php on line 18
bool(true)
NULL

Warning: DOMDocument::load(): supplied argument is not a valid Stream-Context resource in %s004.php on line 18
bool(true)
NULL

Warning: DOMDocument::load(): supplied argument is not a valid Stream-Context resource in %s004.php on line 18
bool(true)
NULL
bool(true)
NULL
bool(true)
NULL
bool(true)
Done
