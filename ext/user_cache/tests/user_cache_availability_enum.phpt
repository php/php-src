--TEST--
UserCache\CacheAvailability: enum case surface stays stable
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php

foreach (UserCache\CacheAvailability::cases() as $case) {
    echo $case->name, "\n";
}
?>
--EXPECT--
Available
DisabledByIni
DisabledBySapi
UnavailableBySharedMemoryInitializationFailed
UnavailableByBackendNotInitializedBeforeWorkerStartup
UnavailableByBackendInitializedAfterWorkerStartup
UnavailableByCgiFastCgiBoundary
UnavailableByLsapiBoundary
UnavailableByUnknownReason
