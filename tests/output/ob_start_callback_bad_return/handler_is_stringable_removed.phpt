--TEST--
ob_start(): Check behaviour with deprecation when OOM triggers handler removal (handler returns stringable object)
--INI--
memory_limit=2M
--FILE--
<?php

class IsStringable {
    public function __construct(public string $val) {}
    public function __toString() {
        return __CLASS__ . ": " . $this->val;
    }
}

ob_start(function() {
    // We are out of memory, now trigger a deprecation
    return new IsStringable("");
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
