--TEST--
libxml_set_streams_context()
--SKIPIF--
<?php if (!extension_loaded('dom')) die('skip dom extension not available'); ?>
--FILE--
<?php

$ctxs = array(
	NULL,
	'bogus',
	123,
	new stdclass,
	array('a'),
	stream_context_create(),
);

foreach ($ctxs as $ctx) {
	try {
        var_dump(libxml_set_streams_context($ctx));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
	$dom = new DOMDocument();
	var_dump($dom->load(__DIR__.'/test.xml'));
}

echo "Done\n";

?>
--EXPECTF--
libxml_set_streams_context() expects parameter 1 to be resource, null given
bool(true)
libxml_set_streams_context() expects parameter 1 to be resource, string given
bool(true)
libxml_set_streams_context() expects parameter 1 to be resource, int given
bool(true)
libxml_set_streams_context() expects parameter 1 to be resource, object given
bool(true)
libxml_set_streams_context() expects parameter 1 to be resource, array given
bool(true)
NULL
bool(true)
Done
