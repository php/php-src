--TEST--
GH-24707 (Use-after-free in opendir() when a user wrapper unregisters itself)
--FILE--
<?php
class Wrapper {
    public $context;

    public function dir_opendir(string $path, int $options): bool {
        stream_wrapper_unregister('foo');
        return false;
    }
}
stream_wrapper_register('foo', 'Wrapper');
var_dump(opendir('foo://'));
?>
--EXPECTF--
Warning: opendir(): Failed to open directory: "Wrapper::dir_opendir" call failed in %s on line %d
bool(false)
