--TEST--
Test Random: NumberGenerator: error: serialize
--FILE--
<?php

foreach (include __DIR__ . DIRECTORY_SEPARATOR . 'common.inc' as $klass => $is_clone_or_serializable) {
    if ($is_clone_or_serializable) {
        continue;
    }

    $instance = new $klass();

    try {
        $clone_instance = unserialize(serialize($instance));
    } catch (Exception $e) {
        continue;
    }

    die('failure: ' . $instance::class);
}

die('success');
?>
--EXPECT--
success
