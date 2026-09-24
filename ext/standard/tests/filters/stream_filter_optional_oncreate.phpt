--TEST--
Stream filters allow absent onCreate and preserve explicit rejection
--FILE--
<?php
class UppercaseFilter {
    public $filtername;
    public $params;

    public function filter($in, $out, &$consumed, bool $closing): int {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            $bucket->data = strtoupper($bucket->data);
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}

class RefusedFilter extends php_user_filter {
    public function onCreate(): bool {
        return false;
    }

    public function __destruct() {
        echo "rejected filter destroyed\n";
    }
}

stream_filter_register('uppercase', UppercaseFilter::class);
stream_filter_register('refused', RefusedFilter::class);
$stream = fopen('php://memory', 'w+');
var_dump(@stream_filter_append($stream, 'refused', STREAM_FILTER_WRITE));
var_dump(is_resource(stream_filter_append($stream, 'uppercase', STREAM_FILTER_WRITE)));
var_dump(fwrite($stream, 'Hello'));
rewind($stream);
var_dump(stream_get_contents($stream));
fclose($stream);
?>
--EXPECT--
rejected filter destroyed
bool(false)
bool(true)
int(5)
string(5) "HELLO"
