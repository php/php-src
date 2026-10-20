--TEST--
Bug GH-22818: user_filter_factory_create assertion failure on shutdown re-registration
--FILE--
<?php

class rotate_filter_nw extends php_user_filter
{
    private static bool $done = false;

    public function filter($in, $out, &$consumed, $closing): int
    {
        if (self::$done) {
            return PSFS_PASS_ON;
        }
        self::$done = true;

        $stream = fopen('php://memory', 'w+');
        var_dump(stream_filter_register("rotator_notWorking", rotate_filter_nw::class));
        var_dump(stream_filter_append($stream, "rotator_notWorking") !== false);

        return PSFS_PASS_ON;
    }
}

stream_filter_register("rotator_notWorking", rotate_filter_nw::class);

$stream = fopen('php://memory', 'w+');
stream_filter_append($stream, "rotator_notWorking");

echo "done\n";
?>
--EXPECT--
done
bool(false)
bool(true)
