--TEST--
Bug #79797 (Use of freed hash key in the phar_parse_zipfile function)
--EXTENSIONS--
phar
--INI--
phar.cache_list={PWD}/bug79797.phar
--FILE--
<?php
echo "done\n";
?>
--EXPECT--
done
