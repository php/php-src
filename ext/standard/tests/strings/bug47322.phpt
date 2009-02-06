--TEST--
Bug #47322 (sscanf %d does't work)
--FILE--
<?php

sscanf("15:59:58.2","%d:%d:%f", $a, $b, $c);
echo "[$a][$b][$c]\n";

sscanf("15:59:58.2","%d:%d:%f", $a, $b, $c);
echo "[$a][$b][$c]\n";

sscanf("15:59:foo","%d:%d:%s", $a, $b, $c);
echo "[$a][$b][$c]\n";

?>
--EXPECT--
[15][59][58.2]
[15][59][58.2]
[15][59][foo]
