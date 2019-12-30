--TEST--
Stats executed during include path resolution should be silent
--FILE--
<?php

class StreamWrapper {
    public function url_stat($path, $flags) {
        $path = str_replace('test://', 'file://', $path);
        if ($flags & STREAM_URL_STAT_QUIET) {
            return @stat($path);
        } else {
            return stat($path);
        }
    }
}

stream_wrapper_register('test', StreamWrapper::class);
set_include_path('test://foo:test://bar');

try {
    require_once 'doesnt_exist.php';
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: require_once(doesnt_exist.php): failed to open stream: No such file or directory in %s on line %d

Fatal error: require_once(): Failed opening required 'doesnt_exist.php' (include_path='test://foo:test://bar') in %s on line %d
