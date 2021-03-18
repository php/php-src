--TEST--
Creating the stream filter object may fail
--FILE--
<?php
try {
    class SampleFilter extends php_user_filter {
        private $data = \FOO;
    }
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
stream_filter_register('sample.filter', SampleFilter::class);
try {
    var_dump(file_get_contents('php://filter/read=sample.filter/resource='. __FILE__));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Undefined constant "FOO"

Warning: file_get_contents(): Unable to create or locate filter "sample.filter" in %s on line %d

Warning: file_get_contents(): Unable to create filter (sample.filter) in %s on line %d
Trying to use class for which initializer evaluation has previously failed
