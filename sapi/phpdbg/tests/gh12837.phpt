--TEST--
GH-12837 (Combination of phpdbg and Generator method causes memory leak)
--CREDITS--
ngyuki
--FILE--
<?php
class A
{
    public function __construct()
    {
        var_dump('__construct');
    }

    public function __destruct()
    {
        var_dump('__destruct');
    }

    public function iterate()
    {
        yield 1;
    }
}

$arr = iterator_to_array((new A())->iterate()); gc_collect_cycles();
$arr = iterator_to_array((new A())->iterate()); gc_collect_cycles();
$arr = iterator_to_array((new A())->iterate()); gc_collect_cycles();

var_dump('exit');
?>
--PHPDBG--
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> string(11) "__construct"
string(10) "__destruct"
string(11) "__construct"
string(10) "__destruct"
string(11) "__construct"
string(10) "__destruct"
string(4) "exit"
[Script ended normally]
prompt>
