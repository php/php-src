--TEST--
Bug #33562 (date("") crashes)
--FILE--
<?php
date_default_timezone_set("GMT");
echo "[", date(""), "]\n";
echo "done";
?>
--EXPECT--
[]
done
