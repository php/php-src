--TEST--
GH-15836 (use-after-free when a user filter reads $this->stream during the close flush)
--FILE--
<?php
class my_filter extends php_user_filter {
    public static ?Throwable $e = null;
    function filter($in, $out, &$consumed, $closing): int {
        if ($closing) {
            try {
                stream_bucket_new($this->stream, "x");
            } catch (TypeError $e) {
                self::$e = $e;
            }
        }
        return PSFS_PASS_ON;
    }
}
var_dump(stream_filter_register("my_filter", "my_filter"));

function run() {
    $s = fopen("php://memory", "wb+");
    stream_filter_append($s, "my_filter", STREAM_FILTER_WRITE);
}
run();

echo my_filter::$e->getTraceAsString(), "\n";
echo "done\n";
?>
--EXPECTF--
bool(true)
#0 %s(%d): stream_bucket_new(NULL, 'x')
#1 %s(%d): my_filter->filter(Resource id #%d, Resource id #%d, 0, true)
#2 {main}
done
