--TEST--
IO hooks: a signal wait the provider keeps reporting as taken elsewhere ends at its deadline
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_sigtimedwait')) die('skip pcntl_sigtimedwait() not available');
?>
--FILE--
<?php
final class Again implements Io\Hooks\Hooks
{
    public function getCapabilities(): array { return []; }
    public function run(Io\Operation $op): Io\Completion
    {
        if ($op instanceof Io\Operation\SigWait) {
            return $op->complete(Io\CompletionStatus::Done, -1, PCNTL_EAGAIN);
        }
        return $op->complete(Io\CompletionStatus::Unsupported);
    }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
}

pcntl_sigprocmask(SIG_BLOCK, [SIGUSR1]);
Io\Hooks\set_hooks(new Again());
var_dump(pcntl_sigtimedwait([SIGUSR1], $info, 0, 1000000));
Io\Hooks\set_hooks(null);
?>
--EXPECT--
bool(false)
