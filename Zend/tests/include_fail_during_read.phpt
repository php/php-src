--TEST--
Include fails during read
--FILE--
<?php
class SampleFilter extends php_user_filter { }
stream_filter_register('sample.filter', SampleFilter::class);
include 'php://filter/read=sample.filter/resource='. __FILE__;
?>
--EXPECTF--
Warning: include(): Unprocessed filter buckets remaining on input brigade in %s on line %d

Warning: include(): Failed opening 'php://filter/read=sample.filter/resource=%s' for inclusion (include_path='%s') in %s on line %d
