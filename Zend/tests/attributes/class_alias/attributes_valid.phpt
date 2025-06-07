--TEST--
Alias attributes do not need to exist for declaration
--FILE--
<?php

#[ClassAlias('Other', [new MissingAttribute()])]
class Demo {}

echo "Done\n"
?>
--EXPECT--
Done
