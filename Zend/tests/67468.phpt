--TEST--
Bug #67468 (Segfault in highlight_file()/highlight_string())
--FILE--
<?php
highlight_string("<?php __CLASS__;", true);
echo "done";
?>
--EXPECT--
done
