--TEST--
Bug #68669 (DateTime::createFromFormat() does not allow NULL $timezone)
--INI--
date.timezone=UTC
--FILE--
<?php

$result = get_class(DateTime::createFromFormat('Y/m/d', '2015/1/1', null));
if ($result == 'DateTime') {
    echo 'Done';
} else {
    echo 'Faild';
}
?>
--EXPECTF--
Done