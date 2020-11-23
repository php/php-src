--TEST--
Constants exported by ext/mysqli - checking category - PHP bug not mysqli bug (check from time to time)
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
    $constants = get_defined_constants(true);

    foreach ($constants as $group => $consts) {
        foreach ($consts as $name => $value) {
            if (stristr($name, 'mysqli')) {
                if ('mysqli' != $group)
                printf("found constant '%s' in group '%s'. expecting group 'mysqli'\n", $name, $group);
            }
        }
    }

    print "done!";
?>
--EXPECT--
done!
