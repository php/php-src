--TEST--
Bug #25145 (SEGV on recpt of form input with name like "123[]")
--GET--
123[]=SEGV
--FILE--
<?php

print_r($_REQUEST);
echo "Done\n";

?>
--EXPECT--
Array
(
    [123] => Array
        (
            [0] => SEGV
        )

)
Done
