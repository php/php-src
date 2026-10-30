--TEST--
Bug #39032 (strcspn() stops on null character)
--FILE--
<?php

var_dump(strcspn(chr(0),"x"));
var_dump(strcspn(chr(0),""));
var_dump(strcspn(chr(0),"qweqwe"));
var_dump(strcspn(chr(1),"qweqwe"));

echo "Done\n";
?>
--EXPECT--
int(1)
int(1)
int(1)
int(1)
Done
