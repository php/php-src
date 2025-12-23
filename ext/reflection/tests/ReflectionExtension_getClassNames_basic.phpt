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
Io\Poll\Backend
Io\Poll\Context
Io\Poll\Event
Io\Poll\Handle
Io\Poll\PollException
Io\Poll\Watcher
RoundingMode
StreamBucket
StreamPollHandle
__PHP_Incomplete_Class
php_user_filter
