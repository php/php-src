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
    /* Only with --with-ior, see ReflectionExtension_getClassNames_ring.phpt */
    if (str_starts_with($className, 'Io\\Ring\\')) {
        continue;
    }
    echo $className, PHP_EOL;
}
?>
--EXPECT--
AssertionError
Directory
Io\Completion
Io\CompletionStatus
Io\Hooks\Capability
Io\Hooks\Hooks
Io\InvalidOperationException
Io\InvalidRegistrationException
Io\IoException
Io\Operation
Io\OperationQueue
Io\Operation\Accept
Io\Operation\Any
Io\Operation\Connect
Io\Operation\Fsync
Io\Operation\GetAddrInfo
Io\Operation\GetNameInfo
Io\Operation\Poll
Io\Operation\Read
Io\Operation\Recv
Io\Operation\Send
Io\Operation\SigWait
Io\Operation\Timer
Io\Operation\WaitPid
Io\Operation\Write
Io\Poll\Backend
Io\Poll\BackendUnavailableException
Io\Poll\Context
Io\Poll\Event
Io\Poll\FailedContextInitializationException
Io\Poll\FailedHandleAddException
Io\Poll\FailedPollOperationException
Io\Poll\FailedPollWaitException
Io\Poll\FailedWatcherModificationException
Io\Poll\Handle
Io\Poll\HandleAlreadyWatchedException
Io\Poll\InactiveWatcherException
Io\Poll\InvalidHandleException
Io\Poll\NotifyHandle
Io\Poll\OperationQueue
Io\Poll\PollException
Io\Poll\ProcessHandle
Io\Poll\SignalHandle
Io\Poll\TimerHandle
Io\Poll\Trigger
Io\Poll\Watcher
Io\Poll\WeakHandle
Io\Registration
RoundingMode
SortDirection
StreamBucket
StreamError
StreamErrorCode
StreamErrorMode
StreamErrorStore
StreamException
StreamPollHandle
StreamPollWeakHandle
__PHP_Incomplete_Class
php_user_filter