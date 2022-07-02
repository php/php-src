--TEST--
Bug #81512: Unexpected behavior with arrays and JIT
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
$pipe = [['val1'],['val2'],];

for ($i = 0; $i < 30; ++$i) {
        echo "$i ";
        if (!is_pipeline($pipe)) {
                echo 'ERROR ';
        }
}

function is_pipeline($pipeline): bool {
        foreach ($pipeline as $stage) {
                if (!is_array($stage)) {
                        return false; // must never happen
                }

                $stage = (array) $stage;
                reset($stage);
        }

        return true;
}
?>
--EXPECT--
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29
