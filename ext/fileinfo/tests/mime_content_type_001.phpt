--TEST--
mime_content_type(): Testing wrong parameters
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

mime_content_type(1);
mime_content_type(NULL);
mime_content_type(new stdclass);
mime_content_type(array());
mime_content_type('foo/inexistent');
mime_content_type('');
mime_content_type("\0");

?>
--EXPECTF--
Warning: mime_content_type(): Can only process string or stream arguments in %s on line %d

Warning: mime_content_type(): Can only process string or stream arguments in %s on line %d

Warning: mime_content_type(): Can only process string or stream arguments in %s on line %d

Warning: mime_content_type(): Can only process string or stream arguments in %s on line %d

Warning: mime_content_type(): File or path not found 'foo/inexistent' in %s on line %d

Warning: mime_content_type(): Empty filename or path in %s on line %d

Warning: mime_content_type(): Empty filename or path in %s on line %d
