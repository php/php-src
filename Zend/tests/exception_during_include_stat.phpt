--TEST--
Make sure exceptions during include/require stating are properly propagated
--FILE--
<?php

class StreamWrapper {
    public $context;
    public function url_stat($path, $flags) {
        throw new Exception('stat failed');
    }
}

stream_wrapper_register('test', StreamWrapper::class);
set_include_path('test://foo:test://bar');

try {
    require_once 'doesnt_exist.php';
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
try {
    require 'doesnt_exist.php';
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
try {
    include_once 'doesnt_exist.php';
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
try {
    include 'doesnt_exist.php';
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
stat failed
stat failed
stat failed
stat failed
