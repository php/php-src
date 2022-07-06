--TEST--
Bug #78506: Error in a php_user_filter::filter() is not reported
--FILE--
<?php

class MyFilter extends php_user_filter {
    public function filter($in, $out, &$consumed, $closing)
    {
        stream_bucket_make_writeable($in);
        return PSFS_ERR_FATAL;
    }

}

stream_filter_register('filtername', MyFilter::class);

$source_resource = fopen('php://memory', 'rb+');
fwrite($source_resource, 'Test data');
rewind($source_resource);

stream_filter_prepend($source_resource,'filtername',STREAM_FILTER_READ);

var_dump(stream_copy_to_stream($source_resource, fopen('php://memory', 'wb')));

?>
--EXPECT--
bool(false)
