--TEST--
"async" is only a keyword before "function"/"fn"; "await" may be used as a member name
--FILE--
<?php
namespace Foo;

function async(callable $cb) { return "userland " . $cb(); }

class Bar {
    const await = 'const';
    public $async = 'prop';
    public function await() { return 'method'; }
    public function async() { return 'method2'; }
}

echo async(fn() => "async()"), "\n";
$b = new Bar;
echo Bar::await, " ", $b->async, " ", $b->await(), " ", $b->async(), "\n";
echo \Async\wait(\Async\async(fn() => "explicit")), "\n";
?>
--EXPECT--
userland async()
const prop method method2
explicit
