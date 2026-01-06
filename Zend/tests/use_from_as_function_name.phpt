--TEST--
`from` remains usable as function name (soft keyword)
--FILE--
<?php

namespace Test;

function from(): void
{
    echo "ok\n";
}

from();

?>
--EXPECT--
ok
