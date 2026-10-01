--TEST--
pcntl_signal(): $restart_syscalls is nullable
--EXTENSIONS--
pcntl
--FILE--
<?php
declare(strict_types=1);

$parameter = (new ReflectionFunction('pcntl_signal'))->getParameters()[2];
var_dump((string) $parameter->getType());
var_dump($parameter->allowsNull());
var_dump($parameter->getDefaultValue());

var_dump(pcntl_signal(SIGALRM, SIG_IGN));
var_dump(pcntl_signal(SIGALRM, SIG_IGN, null));
var_dump(pcntl_signal(SIGALRM, SIG_IGN, true));
var_dump(pcntl_signal(SIGALRM, SIG_IGN, false));

try {
    pcntl_signal(SIGALRM, SIG_IGN, 1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump(pcntl_signal(SIGALRM, SIG_DFL));
?>
--EXPECT--
string(5) "?bool"
bool(true)
NULL
bool(true)
bool(true)
bool(true)
bool(true)
TypeError: pcntl_signal(): Argument #3 ($restart_syscalls) must be of type ?bool, int given
bool(true)
