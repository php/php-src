--TEST--
Test Csv\LazyLaxCollection::createFromFile(): several retained references to the internal stream's resource, released in different orders
--DESCRIPTION--
Regression test for a leak caught by the debug builds of the GitHub CI on the ext/csv pull
request: when userland retained a reference to the internal stream's resource while it was
being opened, the resource was removed from the regular list anyway, so releasing the last
reference never freed the zend_resource. The resource must instead stay listed as a closed
resource until its last reference is released, whatever the order of releases.
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

$file = __DIR__ . '/retained_resource_reference_release_order.csv';
file_put_contents($file, "a,b\r\nc,d\r\n");

function open_collection(string $file): array {
    $collection = Csv\LazyLaxCollection::createFromFile('php://filter/read=grabbing/resource=' . $file);
    $closed = array_values(array_filter(GrabbingFilter::$grabbed, fn($res) => !is_resource($res)));
    GrabbingFilter::$grabbed = [];
    var_dump(count($closed));
    return [$collection, $closed[0]];
}

function closed_ids(): array {
    return array_map('intval', array_values(get_resources('Unknown')));
}

function iterate(Csv\LazyLaxCollection $collection): void {
    foreach ($collection as $row) {
        echo json_encode($row), \PHP_EOL;
    }
}

$baseline = closed_ids();

echo "--- Release one reference, destroy the collection, then release the rest\n";
[$collection, $a] = open_collection($file);
$b = $a;
$holder = new stdClass();
$holder->res = $a;
$id = (int) $a;
var_dump(gettype($a), gettype($b), gettype($holder->res));
var_dump(in_array($id, closed_ids(), true));
/* get_resources() hands out further references; dropping them must not free it early */
$again = get_resources('Unknown');
var_dump(in_array($id, array_map('intval', $again), true));
unset($again);
var_dump(in_array($id, closed_ids(), true));
iterate($collection);
unset($b);
var_dump(in_array($id, closed_ids(), true));
unset($collection);
var_dump(gettype($a), gettype($holder->res));
var_dump(in_array($id, closed_ids(), true));
unset($holder);
var_dump(in_array($id, closed_ids(), true));
unset($a);
var_dump(closed_ids() === $baseline);

echo "--- Release every reference before the collection is used\n";
[$collection, $a] = open_collection($file);
$b = $a;
$id = (int) $a;
unset($a, $b);
var_dump(in_array($id, closed_ids(), true));
iterate($collection);
iterate($collection);
unset($collection);
var_dump(closed_ids() === $baseline);

echo "--- Destroy the collection first, then release references in reverse order\n";
[$collection, $a] = open_collection($file);
$refs = [$a, $a, $a];
$id = (int) $a;
iterate($collection);
unset($collection, $a);
var_dump(in_array($id, closed_ids(), true));
array_pop($refs);
array_pop($refs);
var_dump(gettype($refs[0]));
var_dump(in_array($id, closed_ids(), true));
array_pop($refs);
var_dump(closed_ids() === $baseline);

echo "done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/retained_resource_reference_release_order.csv');
?>
--EXPECT--
--- Release one reference, destroy the collection, then release the rest
int(1)
string(17) "resource (closed)"
string(17) "resource (closed)"
string(17) "resource (closed)"
bool(true)
bool(true)
bool(true)
["a","b"]
["c","d"]
bool(true)
string(17) "resource (closed)"
string(17) "resource (closed)"
bool(true)
bool(true)
bool(true)
--- Release every reference before the collection is used
int(1)
bool(false)
["a","b"]
["c","d"]
["a","b"]
["c","d"]
bool(true)
--- Destroy the collection first, then release references in reverse order
int(1)
["a","b"]
["c","d"]
bool(true)
string(17) "resource (closed)"
bool(true)
bool(true)
done
