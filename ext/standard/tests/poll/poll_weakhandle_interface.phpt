--TEST--
Io\Poll: WeakHandle is the type of handles that hold their resource weakly
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$r, $w] = pt_new_socket_pair();
var_dump(StreamPollWeakHandle::create($r) instanceof Io\Poll\WeakHandle);
var_dump(StreamPollWeakHandle::create($r) instanceof Io\Poll\Handle);
var_dump(new StreamPollHandle($r) instanceof Io\Poll\WeakHandle);
var_dump((new ReflectionClass(Io\Poll\WeakHandle::class))->isInterface());
var_dump(class_implements(Io\Poll\WeakHandle::class));

try {
    eval('class MyWeak implements Io\Poll\WeakHandle {}');
} catch (Error $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(false)
bool(true)
array(1) {
  ["Io\Poll\Handle"]=>
  string(14) "Io\Poll\Handle"
}

Fatal error: Io\Poll\Handle cannot be implemented by user classes in %s on line %d
