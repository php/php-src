--TEST--
Test Csv\LazyLaxCollection::createFromFile(): several retained references to the internal stream's resource, released in different orders
--DESCRIPTION--
Userland can retain references to the resource of the internal stream (here captured while the
stream is being opened). The stream stays open while the collection is alive, becomes a closed
resource once the collection is destroyed, and the resource is freed when its last reference
is released, whatever the order of releases.
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
    $before = array_map('intval', get_resources('stream'));
    $collection = Csv\LazyLaxCollection::createFromFile('php://filter/read=grabbing/resource=' . $file);
    $internal = array_values(array_filter(GrabbingFilter::$grabbed, fn($res) => !in_array((int) $res, $before, true)));
    GrabbingFilter::$grabbed = [];
    var_dump(count($internal));
    return [$collection, $internal[0]];
}

/* "stream" while open, "closed" once closed but still referenced, "freed" afterwards */
function state(int $id): string {
    if (in_array($id, array_map('intval', get_resources('stream')), true)) {
        return 'stream';
    }
    if (in_array($id, array_map('intval', get_resources('Unknown')), true)) {
        return 'closed';
    }
    return in_array($id, array_map('intval', get_resources()), true) ? 'other' : 'freed';
}

function iterate(Csv\LazyLaxCollection $collection): void {
    foreach ($collection as $row) {
        echo json_encode($row), \PHP_EOL;
    }
}

echo "--- Release one reference, destroy the collection, then release the rest\n";
[$collection, $a] = open_collection($file);
$b = $a;
$holder = new stdClass();
$holder->res = $a;
$id = (int) $a;
echo state($id), \PHP_EOL;
/* get_resources() hands out further references; dropping them must not free it early */
$again = get_resources('stream');
unset($again);
echo state($id), \PHP_EOL;
iterate($collection);
unset($b);
echo state($id), \PHP_EOL;
unset($collection);
var_dump(gettype($a), gettype($holder->res));
echo state($id), \PHP_EOL;
unset($holder);
echo state($id), \PHP_EOL;
unset($a);
echo state($id), \PHP_EOL;

echo "--- Release every reference before the collection is used\n";
[$collection, $a] = open_collection($file);
$b = $a;
$id = (int) $a;
unset($a, $b);
echo state($id), \PHP_EOL;
iterate($collection);
iterate($collection);
unset($collection);
echo state($id), \PHP_EOL;

echo "--- Destroy the collection first, then release references in reverse order\n";
[$collection, $a] = open_collection($file);
$refs = [$a, $a, $a];
$id = (int) $a;
iterate($collection);
unset($collection, $a);
echo state($id), \PHP_EOL;
array_pop($refs);
array_pop($refs);
var_dump(gettype($refs[0]));
echo state($id), \PHP_EOL;
array_pop($refs);
echo state($id), \PHP_EOL;

echo "done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/retained_resource_reference_release_order.csv');
?>
--EXPECT--
--- Release one reference, destroy the collection, then release the rest
int(1)
stream
stream
["a","b"]
["c","d"]
stream
string(17) "resource (closed)"
string(17) "resource (closed)"
closed
closed
freed
--- Release every reference before the collection is used
int(1)
stream
["a","b"]
["c","d"]
["a","b"]
["c","d"]
freed
--- Destroy the collection first, then release references in reverse order
int(1)
["a","b"]
["c","d"]
closed
string(17) "resource (closed)"
closed
freed
done
