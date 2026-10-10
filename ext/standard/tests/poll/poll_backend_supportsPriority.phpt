--TEST--
Io\Poll\Backend::supportsPriority(): kqueue, WSAPoll and Darwin's poll() cannot report Event::Priority
--FILE--
<?php
foreach (Io\Poll\Backend::cases() as $backend) {
    if ($backend === Io\Poll\Backend::Auto) {
        continue;
    }
    if (!$backend->isAvailable()) {
        var_dump(true);
        continue;
    }
    $expected = match ($backend) {
        Io\Poll\Backend::Kqueue, Io\Poll\Backend::WSAPoll => false,
        Io\Poll\Backend::Poll => PHP_OS_FAMILY !== 'Darwin',
        default => true,
    };
    var_dump($backend->supportsPriority() === $expected);
}
var_dump(is_bool(Io\Poll\Backend::Auto->supportsPriority()));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
