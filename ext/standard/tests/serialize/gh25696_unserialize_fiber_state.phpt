--TEST--
GH-25696: unserialize state survives a Fiber suspended during nested unserialization
--FILE--
<?php
error_reporting(E_ALL & ~E_DEPRECATED);

function autoload($class) {
    Fiber::suspend();
}

ini_set('unserialize_callback_func', 'autoload');

class Outer implements Serializable {
    public function serialize(): string {
        return '';
    }

    public function unserialize($data): void {
        $GLOBALS['fiber']->start();
    }
}

$GLOBALS['fiber'] = new Fiber(function () {
    @unserialize('O:7:"Missing":0:{}');
});

$outer = unserialize(serialize(new Outer));
$GLOBALS['fiber']->resume();
var_dump(unserialize('i:1;'));
?>
--EXPECT--
int(1)
