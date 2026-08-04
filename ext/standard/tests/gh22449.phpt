--TEST--
Bug GH-22449: user_filter_factory_create NULL dereference during shutdown
--FILE--
<?php

class rotate_filter_nw extends php_user_filter
{
    public function filter($in, $out, &$consumed, $closing): int
    {
        $stream = fopen('php://memory', 'w+');
        stream_filter_append($stream, "rotator_notWorking");

        return PSFS_PASS_ON;
    }
}

stream_filter_register("rotator_notWorking", rotate_filter_nw::class);

$stream = fopen('php://memory', 'w+');
stream_filter_append($stream, "rotator_notWorking");

echo "done\n";
?>
--EXPECTF--
done

Warning: stream_filter_append(): Unable to create or locate filter "rotator_notWorking" in %s on line %d
