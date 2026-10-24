--TEST--
Test Csv\collection_to_file(): retained references to the internal stream's resource are released safely
--DESCRIPTION--
Regression test for a leak caught by the debug builds of the GitHub CI on the ext/csv pull
request (see LazyCollection/fromFile/retained_resource_reference_release_order.phpt):
collection_to_file() makes its stream private the same way, so a resource reference retained
while the stream is being opened must remain a closed resource until it is released.
--EXTENSIONS--
csv
--FILE--
<?php
class GrabbingFilter extends php_user_filter {
    public static array $grabbed = [];
    public function onCreate(): bool {
        /* Runs while the stream is being opened: capture every stream resource */
        self::$grabbed = get_resources('stream');
        return true;
    }
    public function filter($in, $out, &$consumed, $closing): int {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}
stream_filter_register('grabbing', GrabbingFilter::class);

function closed_ids(): array {
    return array_map('intval', array_values(get_resources('Unknown')));
}

$file = __DIR__ . '/retained_resource_reference_to_file.csv';
$baseline = closed_ids();

Csv\collection_to_file('php://filter/write=grabbing/resource=' . $file, [['a', 'b'], ['c', 'd']]);
$closed = array_values(array_filter(GrabbingFilter::$grabbed, fn($res) => !is_resource($res)));
GrabbingFilter::$grabbed = [];
var_dump(count($closed));
$a = $closed[0];
$b = $a;
unset($closed);
$id = (int) $a;
var_dump(gettype($a));
var_dump(in_array($id, closed_ids(), true));
var_dump(file_get_contents($file));
unset($a);
var_dump(in_array($id, closed_ids(), true));
unset($b);
var_dump(closed_ids() === $baseline);
echo "done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/retained_resource_reference_to_file.csv');
?>
--EXPECT--
int(1)
string(17) "resource (closed)"
bool(true)
string(10) "a,b
c,d
"
bool(true)
bool(true)
done
