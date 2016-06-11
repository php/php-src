--TEST--
goto inside foreach
--FILE--
<?php

foreach ([0] as $x) {
    goto a;
a:
    echo "loop\n";
}

echo "done\n";
?>
--EXPECT--
loop
done
