--TEST--
json_encode() OOM while escaping a property name releases the recursion guard of an object with hooked properties
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public string $a { get => str_repeat('v', 100); }
}

$o = new C;
$o->{str_repeat('k', 3 * 1024 * 1024)} = 1;

register_shutdown_function(function () use ($o) {
    ini_set('memory_limit', '-1');
    $json = json_encode($o);
    var_dump($json !== false, json_last_error_msg());
});

// Leave less than the 3 MiB property name needs, so that pre-allocating it
// fails while escaping the key, after the object was marked as being encoded.
ini_set('memory_limit', memory_get_usage(true) + 1024 * 1024);
json_encode($o);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
%Abool(true)
string(8) "No error"
