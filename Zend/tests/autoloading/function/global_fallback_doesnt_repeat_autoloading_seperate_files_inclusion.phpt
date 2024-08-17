--TEST--
Fallback to global function should trigger autoloading only once per namespace, seperate files case.
--FILE--
<?php

function loader($name) {
    echo "function loader called with $name\n";
}

autoload_register_function('loader');

function foo() {
    echo "I am foo in global namespace.\n";
}

include __DIR__ . DIRECTORY_SEPARATOR . 'global_fallback_doesnt_repeat_autoloading_file1.inc';
include __DIR__ . DIRECTORY_SEPARATOR . 'global_fallback_doesnt_repeat_autoloading_file2.inc';

?>
--EXPECT--
First file START
function loader called with bar\foo
I am foo in global namespace.
I am foo in global namespace.
I am foo in global namespace.
I am foo in global namespace.
First file END
Second file START
I am foo in global namespace.
Second file END
