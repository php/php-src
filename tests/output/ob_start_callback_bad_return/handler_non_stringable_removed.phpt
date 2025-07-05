--TEST--
ob_start(): Check behaviour with deprecation when OOM triggers handler removal (handler returns non-stringable object)
--INI--
memory_limit=2M
--FILE--
<?php

class NotStringable {
    public function __construct(public string $val) {}
}

ob_start(function() {
    // We are out of memory, now trigger a deprecation
    return new NotStringable("");
});

$a = [];
// trigger OOM in a resize operation
while (1) {
    $a[] = 1;
}

?>
--EXPECTF--
Deprecated: main(): Returning a non-string result from user output handler {closure:%s:%d} is deprecated in %s on line %d

Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d

Fatal error: Uncaught Error: Object of class NotStringable could not be converted to string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
