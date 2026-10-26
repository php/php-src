--TEST--
User stream wrapper unregistered by its own constructor or destructor during a URL operation
--FILE--
<?php
class Ctor {
    public $context;
    public function __construct() { stream_wrapper_unregister('foo'); }
}
class Dtor {
    public $context;
    public function __destruct() { stream_wrapper_unregister('foo'); }
}

$ops = [
    'unlink' => fn() => unlink('foo://a'),
    'rename' => fn() => rename('foo://a', 'foo://b'),
    'mkdir' => fn() => mkdir('foo://a'),
    'rmdir' => fn() => rmdir('foo://a'),
    'touch' => fn() => touch('foo://a'),
    'chmod' => fn() => chmod('foo://a', 0644),
    'stat' => fn() => stat('foo://a'),
];

foreach (['Ctor', 'Dtor'] as $class) {
    foreach ($ops as $name => $op) {
        stream_wrapper_register('foo', $class);
        echo "$class $name: ";
        var_dump($op());
        var_dump(in_array('foo', stream_get_wrappers()));
    }
}
?>
--EXPECTF--
Ctor unlink: 
Warning: unlink(): Ctor::unlink is not implemented! in %s on line %d
bool(false)
bool(false)
Ctor rename: 
Warning: rename(): Ctor::rename is not implemented! in %s on line %d
bool(false)
bool(false)
Ctor mkdir: 
Warning: mkdir(): Ctor::mkdir is not implemented! in %s on line %d
bool(false)
bool(false)
Ctor rmdir: 
Warning: rmdir(): Ctor::rmdir is not implemented! in %s on line %d
bool(false)
bool(false)
Ctor touch: 
Warning: touch(): Ctor::stream_metadata is not implemented! in %s on line %d
bool(false)
bool(false)
Ctor chmod: 
Warning: chmod(): Ctor::stream_metadata is not implemented! in %s on line %d
bool(false)
bool(false)
Ctor stat: 
Warning: stat(): Ctor::url_stat is not implemented! in %s on line %d

Warning: stat(): stat failed for foo://a in %s on line %d
bool(false)
bool(false)
Dtor unlink: 
Warning: unlink(): Dtor::unlink is not implemented! in %s on line %d
bool(false)
bool(false)
Dtor rename: 
Warning: rename(): Dtor::rename is not implemented! in %s on line %d
bool(false)
bool(false)
Dtor mkdir: 
Warning: mkdir(): Dtor::mkdir is not implemented! in %s on line %d
bool(false)
bool(false)
Dtor rmdir: 
Warning: rmdir(): Dtor::rmdir is not implemented! in %s on line %d
bool(false)
bool(false)
Dtor touch: 
Warning: touch(): Dtor::stream_metadata is not implemented! in %s on line %d
bool(false)
bool(false)
Dtor chmod: 
Warning: chmod(): Dtor::stream_metadata is not implemented! in %s on line %d
bool(false)
bool(false)
Dtor stat: 
Warning: stat(): Dtor::url_stat is not implemented! in %s on line %d

Warning: stat(): stat failed for foo://a in %s on line %d
bool(false)
bool(false)
