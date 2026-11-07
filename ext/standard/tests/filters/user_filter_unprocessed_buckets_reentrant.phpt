--TEST--
Buckets appended to the input brigade by an error handler during the unprocessed buckets warning do not leak
--FILE--
<?php
class reentrant extends php_user_filter {
    public static bool $armed = false;
    public function filter($in, $out, &$consumed, bool $closing): int {
        if (!self::$armed) {
            self::$armed = true;
            $GLOBALS['brigade'] = $in;
            $GLOBALS['bucket'] = stream_bucket_new($this->stream, "refilled");
        }
        return PSFS_PASS_ON;
    }
}
stream_filter_register("reentrant", "reentrant");

set_error_handler(function (int $severity, string $message): bool {
    if (str_contains($message, "Unprocessed filter buckets")) {
        stream_bucket_append($GLOBALS['brigade'], $GLOBALS['bucket']);
        echo "Handled warning\n";
        return true;
    }
    return false;
});

$fp = fopen("php://memory", "w+");
stream_filter_append($fp, "reentrant", STREAM_FILTER_WRITE);
var_dump(fwrite($fp, "input"));
fclose($fp);
?>
--EXPECT--
Handled warning
int(0)
