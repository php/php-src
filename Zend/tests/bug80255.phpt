--TEST--
Bug #80255: Opcache bug (bad condition result) in 8.0.0rc1
--FILE--
<?php

function test($a, $b, $c) {
    do {
        if ($a && !$b) {
            break;
        } else if ($b) {
            echo "foo\n";
        }
        echo "bar\n";
    } while ($c);
    echo "baz\n";
}

test(true, true, false);

?>
--EXPECT--
foo
bar
baz
