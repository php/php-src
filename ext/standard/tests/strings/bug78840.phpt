--TEST--
Bug #78840 (imploding $GLOBALS crashes)
--FILE--
<?php
$glue = '';
@implode($glue, $GLOBALS);
echo "done\n";
?>
--EXPECT--
done
