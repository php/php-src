--TEST--
Object free handler must call zend_weakrefs_notify
--FILE--
<?php

$classes = get_declared_classes();
$testedClasses = 0;

foreach ($classes as $class) {
    $reflector = new \ReflectionClass($class);
    try {
        $instance = $reflector->newInstanceWithoutConstructor();
    } catch (\Throwable $e) {
        continue;
    }
    $testedClasses++;
    $ref = WeakReference::create($instance);
    $instance = null;
    gc_collect_cycles();
    if ($ref->get() !== null) {
        printf("free handler of %s did not call zend_weakrefs_notify?\n", $class);
    }
}

if ($testedClasses === 0) {
    print "Did not test any class\n";
}
?>
==DONE==
--EXPECT--
==DONE==
