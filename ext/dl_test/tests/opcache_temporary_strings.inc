<?php
try {
    dl_test_test2([]);
} catch (TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

try {
    (new DlTest)->test([]);
} catch (TypeError $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

var_dump((new ReflectionFunction('dl_test_test2'))->getName());

var_dump(get_object_vars(new DlTestSubClass));
