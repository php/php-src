--TEST--
Bug #48725 (Support for flushing in zlib stream, more generic issue)
--FILE--
<?php
class DebugClosingFilter extends php_user_filter {
	public function filter($in, $out, &$consumed, $closing) {
		if ($closing) {
			echo "Closing" . PHP_EOL;
		}
		while ($bucket = stream_bucket_make_writeable($in)) {
			$consumed += $bucket->datalen;
			stream_bucket_append($out, $bucket);
		}
		return PSFS_PASS_ON;
	}
}

stream_filter_register('debug.closing', 'DebugClosingFilter');

$stream = fopen('data://text/plain;base64,' . base64_encode('Foo bar baz'), 'r');
stream_filter_append($stream, 'debug.closing', STREAM_FILTER_READ);
$stream_contents = stream_get_contents($stream);
print $stream_contents . PHP_EOL;
?>
===DONE===
--EXPECT--
Closing
Foo bar baz
===DONE===
