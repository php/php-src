--TEST--
OPcache static cache init hooks are request guarded
--SKIPIF--
<?php
$zendDir = dirname(__DIR__);
foreach (['zend_object_handlers.c', 'zend_vm_def.h', 'zend_vm_execute.h'] as $file) {
    if (!is_file($zendDir . '/' . $file)) {
        die('skip source tree required');
    }
}
?>
--FILE--
<?php

function requireGuardedHook(string $source, string $file, string $hook, int $minimum): void
{
    $unguarded = 'UNEXPECTED(' . $hook . ' != NULL)';
    if (str_contains($source, $unguarded)) {
        throw new RuntimeException($file . ' calls ' . $hook . ' without the request guard');
    }

    $pattern = '/EG\\(static_cache_class_access_active\\) &&\\s+' . preg_quote($hook, '/') . ' != NULL\\)\\s*\\)?\\s*\\{\\s+' . preg_quote($hook, '/') . '\\(/s';
    if (preg_match_all($pattern, $source) < $minimum) {
        throw new RuntimeException($file . ' is missing the guarded ' . $hook . ' calls');
    }
}

$zendDir = dirname(__DIR__);

requireGuardedHook(
    file_get_contents($zendDir . '/zend_object_handlers.c'),
    'zend_object_handlers.c',
    'zend_class_init_statics_hook',
    1
);

foreach (['zend_vm_def.h' => 2, 'zend_vm_execute.h' => 4] as $file => $minimum) {
    requireGuardedHook(
        file_get_contents($zendDir . '/' . $file),
        $file,
        'zend_function_init_statics_hook',
        $minimum
    );
}

echo "OK\n";

?>
--EXPECT--
OK
