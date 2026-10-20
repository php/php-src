--TEST--
Bug GH-23898: user_filter_factory_create assertion failure on shutdown registration of a new filter
--FILE--
<?php

class shutdown_filter extends php_user_filter
{
    private static bool $done = false;

    public function filter($in, $out, &$consumed, $closing): int
    {
        if (self::$done) {
            return PSFS_PASS_ON;
        }
        self::$done = true;

        $stream = fopen('php://memory', 'w+');
        var_dump(stream_filter_register("shutdown_filter_other", shutdown_filter::class));
        var_dump(stream_filter_append($stream, "shutdown_filter") !== false);
        var_dump(stream_filter_append($stream, "shutdown_filter_other") !== false);

        return PSFS_PASS_ON;
    }
}

stream_filter_register("shutdown_filter", shutdown_filter::class);

$stream = fopen('php://memory', 'w+');
stream_filter_append($stream, "shutdown_filter");

echo "done\n";
?>
--EXPECT--
done
bool(true)
bool(true)
bool(true)
