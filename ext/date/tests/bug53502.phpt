--TEST--
Bug #53502 (strtotime with timezone memory leak)
--INI--
date.timezone=UTC
--FILE--
<?php
for ($i = 0; $i < 1000; $i++) {
    strtotime('Monday 00:00 Europe/Paris');    // Memory leak
}
echo "Nothing, test only makes sense through valgrind.\n";
?>
--EXPECT--
Nothing, test only makes sense through valgrind.
