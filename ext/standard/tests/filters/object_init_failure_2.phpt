--TEST--
Creating the stream filter object may fail (include variation)
--FILE--
<?php
class SampleFilter extends php_user_filter {
    private $data = \FOO;
}
stream_filter_register('sample.filter', SampleFilter::class);
try {
    include 'php://filter/read=sample.filter/resource='. __FILE__;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: main(): unable to create or locate filter "sample.filter" in %s on line %d

Warning: main(): Unable to create filter (sample.filter) in %s on line %d
Undefined constant 'FOO'
