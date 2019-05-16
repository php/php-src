--TEST--
Bug #61367: open_basedir bypass in libxml RSHUTDOWN: write test
--SKIPIF--
<?php if(!extension_loaded('dom')) echo 'skip'; ?>
--INI--
open_basedir=.
--FILE--
<?php

class StreamExploiter {
	public function stream_close (  ) {
		$doc = new DOMDocument;
		$doc->appendChild($doc->createTextNode('hello'));
		var_dump($doc->save(dirname(getcwd()) . '/bad'));
	}

	public function stream_open (  $path ,  $mode ,  $options ,  &$opened_path ) {
		return true;
	}
}

var_dump(mkdir('test_bug_61367-write'));
var_dump(mkdir('test_bug_61367-write/base'));
var_dump(file_put_contents('test_bug_61367-write/bad', 'blah'));
var_dump(chdir('test_bug_61367-write/base'));

stream_wrapper_register( 'exploit', 'StreamExploiter' );
$s = fopen( 'exploit://', 'r' );

?>
--CLEAN--
<?php
@unlink('test_bug_61367-write/bad');
rmdir('test_bug_61367-write/base');
rmdir('test_bug_61367-write');
?>
--EXPECTF--
bool(true)
bool(true)
int(4)
bool(true)

Warning: DOMDocument::save(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (.) in %s on line %d

Warning: DOMDocument::save(%s): failed to open stream: Operation not permitted in %s on line %d
bool(false)
