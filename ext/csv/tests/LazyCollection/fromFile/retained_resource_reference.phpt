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

$collection = Csv\LazyLaxCollection::createFromFile('php://filter/read=grabbing/resource=' . $file);
/* The captured reference to the internal stream must now be a closed resource,
 * not a live handle and not freed memory. */
$dead = 0;
foreach ($GLOBALS['grabbed'] as $res) {
    if (!is_resource($res)) {
        $dead++;
    }
}
var_dump($dead);
foreach ($collection as $row) {
    echo json_encode($row), \PHP_EOL;
}
unset($collection, $GLOBALS['grabbed']);
echo "done", \PHP_EOL;
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/retained_resource_reference.csv');
?>
--EXPECT--
int(1)
["a","b"]
["c","d"]
done
