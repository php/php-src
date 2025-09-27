--TEST--
ReflectionExtension::getClassNames() method on an extension which actually returns some information
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php
$standard = new ReflectionExtension('standard');
$classNames = $standard->getClassNames();
sort($classNames);
foreach ($classNames as $className) {
    echo $className, PHP_EOL;
}
?>
--EXPECT--
AssertionError
Directory
PollContext
PollException
PollHandle
PollWatcher
RoundingMode
StreamBucket
StreamPollHandle
__PHP_Incomplete_Class
php_user_filter
