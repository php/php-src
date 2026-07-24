--TEST--
GH-22845 (user filter bucket brigade resource is closed when the callback bails out)
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (!function_exists('zend_trigger_bailout')) die("skip zend_trigger_bailout() not available before PHP 8.5");
?>
--FILE--
<?php
class bf extends php_user_filter {
    public function filter($in, $out, &$consumed, $closing): int {
        $GLOBALS['stash_in'] = $in;
        while ($b = stream_bucket_make_writeable($in)) {
            $consumed += strlen($b->data);
            stream_bucket_append($out, $b);
        }
        register_shutdown_function(function() {
            try {
                stream_bucket_make_writeable($GLOBALS['stash_in']);
                echo "shutdown: no error\n";
            } catch (\Throwable $e) {
                echo "shutdown: ", $e::class, "\n";
            }
        });
        zend_trigger_bailout();
    }
}
stream_filter_register('bf', 'bf');

$fp = fopen('php://memory', 'r+');
fwrite($fp, "hello world");
rewind($fp);
stream_filter_append($fp, 'bf', STREAM_FILTER_READ);
fread($fp, 100);
?>
--EXPECTF--
Fatal error: Bailout in %s on line %d
shutdown: TypeError
