--TEST--
Filter errors: user filter
--FILE--
<?php
require 'filter_errors.inc';

class test_filter0 extends php_user_filter {
	function filter($in, $out, &$consumed, $closing) {
		return PSFS_ERR_FATAL;
	}
}
class test_filter1 extends php_user_filter {
	function filter($in, $out, &$consumed, $closing) {
		$bucket = stream_bucket_make_writeable($in);
		return PSFS_ERR_FATAL;
	}
}
class test_filter2 extends php_user_filter {
	function filter($in, $out, &$consumed, $closing) {
		while ($bucket = stream_bucket_make_writeable($in)) {
			$consumed += $bucket->datalen;
			stream_bucket_append($out, $bucket);
		}
		return PSFS_ERR_FATAL;
	}
}
class test_filter3 extends php_user_filter {
	function filter($in, $out, &$consumed, $closing) {
		if (!$closing) {
			$bucket = stream_bucket_new($this->stream, "42");
			stream_bucket_append($out, $bucket);
		}
		return PSFS_ERR_FATAL;
	}
}
class test_filter4 extends php_user_filter {
	function filter($in, $out, &$consumed, $closing) {
		if (!$closing) {
			$bucket = stream_bucket_new($this->stream, "42");
		}
		return PSFS_ERR_FATAL;
	}
}

for($i = 0; $i < 5; ++$i) {
	echo "test_filter$i\n";
	var_dump(stream_filter_register("test_filter$i", "test_filter$i"));
	filter_errors_test("test_filter$i", "42");
}

echo "test append / read / remove\n";
for($i = 0; $i < 5; ++$i) {
	echo "test_filter$i\n";
	$stream = fopen('php://memory', 'wb+');
	fwrite($stream, "42");
	fseek($stream, 0, SEEK_SET);
	$f = stream_filter_append($stream, "test_filter$i");
	stream_get_contents($stream);
	stream_filter_remove($f);
}

echo "test append all / read / remove all\n";
$stream = fopen('php://memory', 'wb+');
fwrite($stream, "42");
fseek($stream, 0, SEEK_SET);
$filters = array();
for($i = 0; $i < 5; ++$i) {
	echo "test_filter$i\n";
	$filters[] = stream_filter_append($stream, "test_filter$i");
}
stream_get_contents($stream);
foreach($filters as $filter) {
	stream_filter_remove($filter);
}

echo "test append all / read / close\n";
$stream = fopen('php://memory', 'wb+');
fwrite($stream, "42");
fseek($stream, 0, SEEK_SET);
$filters = array();
for($i = 0; $i < 5; ++$i) {
	echo "test_filter$i\n";
	$filters[] = stream_filter_append($stream, "test_filter$i");
}
stream_get_contents($stream);
fclose($stream);

?>
--EXPECTF--
test_filter0
bool(true)
test filtering of buffered data

Warning: stream_filter_append(): Unprocessed filter buckets remaining on input brigade in %s

Warning: stream_filter_append(): Filter failed to process pre-buffered data in %s
test filtering of non buffered data

Warning: stream_get_contents(): Unprocessed filter buckets remaining on input brigade in %s
test_filter1
bool(true)
test filtering of buffered data

Warning: stream_filter_append(): Filter failed to process pre-buffered data in %s
test filtering of non buffered data
test_filter2
bool(true)
test filtering of buffered data

Warning: stream_filter_append(): Filter failed to process pre-buffered data in %s
test filtering of non buffered data
test_filter3
bool(true)
test filtering of buffered data

Warning: stream_filter_append(): Unprocessed filter buckets remaining on input brigade in %s

Warning: stream_filter_append(): Filter failed to process pre-buffered data in %s
test filtering of non buffered data

Warning: stream_get_contents(): Unprocessed filter buckets remaining on input brigade in %s
test_filter4
bool(true)
test filtering of buffered data

Warning: stream_filter_append(): Unprocessed filter buckets remaining on input brigade in %s

Warning: stream_filter_append(): Filter failed to process pre-buffered data in %s
test filtering of non buffered data

Warning: stream_get_contents(): Unprocessed filter buckets remaining on input brigade in %s
test append / read / remove
test_filter0

Warning: stream_get_contents(): Unprocessed filter buckets remaining on input brigade in %s

Warning: stream_filter_remove(): Unable to flush filter, not removing in %s
test_filter1

Warning: stream_filter_remove(): Unable to flush filter, not removing in %s
test_filter2

Warning: stream_filter_remove(): Unable to flush filter, not removing in %s
test_filter3

Warning: stream_get_contents(): Unprocessed filter buckets remaining on input brigade in %s

Warning: stream_filter_remove(): Unable to flush filter, not removing in %s
test_filter4

Warning: stream_get_contents(): Unprocessed filter buckets remaining on input brigade in %s

Warning: stream_filter_remove(): Unable to flush filter, not removing in %s
test append all / read / remove all
test_filter0
test_filter1
test_filter2
test_filter3
test_filter4

Warning: stream_get_contents(): Unprocessed filter buckets remaining on input brigade in %s

Warning: stream_filter_remove(): Unable to flush filter, not removing in %s

Warning: stream_filter_remove(): Unable to flush filter, not removing in %s

Warning: stream_filter_remove(): Unable to flush filter, not removing in %s

Warning: stream_filter_remove(): Unable to flush filter, not removing in %s

Warning: stream_filter_remove(): Unable to flush filter, not removing in %s
test append all / read / close
test_filter0
test_filter1
test_filter2
test_filter3
test_filter4

Warning: stream_get_contents(): Unprocessed filter buckets remaining on input brigade in %s
