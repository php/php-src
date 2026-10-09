--TEST--
Test Csv\LazyLaxCollection::createFromFile(): a resource reference retained during opening stays safe
--EXTENSIONS--
csv
--FILE--
<?php
class GrabbingFilter extends php_user_filter {
    public function onCreate(): bool {
        /* Runs while the stream is being opened: capture every stream resource */
        $GLOBALS['grabbed'] = get_resources('stream');
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

$file = __DIR__ . '/retained_resource_reference.csv';
file_put_contents($file, "a,b\r\nc,d\r\n");

$before = array_map('intval', get_resources('stream'));
$collection = Csv\LazyLaxCollection::createFromFile('php://filter/read=grabbing/resource=' . $file);
$internal = array_values(array_filter($GLOBALS['grabbed'], fn($res) => !in_array((int) $res, $before, true)));
unset($GLOBALS['grabbed']);
var_dump(count($internal));
$res = $internal[0];
unset($internal);

/* The captured reference is the live internal stream, which userland cannot close */
var_dump(gettype($res));
var_dump(fclose($res));
foreach ($collection as $row) {
    echo json_encode($row), \PHP_EOL;
}
/* Destroying the collection closes the stream; the reference sees a closed resource */
unset($collection);
var_dump(gettype($res));
$id = (int) $res;
var_dump(in_array($id, array_map('intval', get_resources('Unknown')), true));
/* Releasing the last reference frees the resource */
unset($res);
var_dump(in_array($id, array_map('intval', get_resources()), true));
echo "done", \PHP_EOL;
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/retained_resource_reference.csv');
?>
--EXPECTF--
int(1)
string(8) "resource"

Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
bool(false)
["a","b"]
["c","d"]
string(17) "resource (closed)"
bool(true)
bool(false)
done
