--TEST--
Combination of foreach, finally and goto
--FILE--
<?php
foreach ([new stdClass] as $a) {
    try {
        foreach ([new stdClass] as $a) {
            try {
                foreach ([new stdClass] as $a) {
                    goto out;
                }
            } finally {
                echo "finally1\n";
            }
out: ;
        }
    } finally {
        echo "finally2\n";
    }
}
?>
--EXPECT--
finally1
finally2
