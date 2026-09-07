--TEST--
file_put_contents LOCK_EX on non-file URL balances stream error operation
--INI--
allow_url_fopen=1
--FILE--
<?php
for ($i = 0; $i < 1005; $i++) {
    @file_put_contents('http://127.0.0.1/x', 'data', LOCK_EX);
}
set_error_handler(function (int $errno, string $errstr): bool {
    if (str_contains($errstr, 'Stream error operation depth exceeded')) {
        echo "depth_exceeded\n";
    }
    return true;
});
file_put_contents('http://127.0.0.1/x', 'data', LOCK_EX);
echo "done\n";
?>
--EXPECT--
done
