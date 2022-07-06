--TEST--
Bug #74606 (Segfault within try/catch/finally nesting in Generators)
--FILE--
<?php

function gen() {
    $array = ["foo"];
    $array[] = "bar";

    foreach ($array as $item) {
        try {
            try {
                yield;
            } finally {
                echo "fin $item\n";
            }
        } catch (\Exception $e) {
            echo "catch\n";
            continue;
        }
    }
}
gen()->throw(new Exception);

?>
--EXPECT--
fin foo
catch
fin bar
