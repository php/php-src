--TEST--
getopt() captures arguments before string conversion modifies argv
--INI--
register_argc_argv=Off
--FILE--
<?php
$argv = ['test', new class {
    public function __toString(): string {
        $GLOBALS['argv'][2] = '-c';
        return '-a';
    }
}, '-b'];
var_dump(getopt('abc'));

$argv = ['test', new class {
    public function __toString(): string {
        unset($GLOBALS['argv']);
        return '-a';
    }
}, '-b'];
var_dump(getopt('abc'));

$a = ['test', new class {
    public function __toString(): string {
        unset($GLOBALS['argv']);
        var_dump(gc_collect_cycles());
        return '-a';
    }
}, '-b'];
$a[3] = &$a;
$argv = $a;
unset($a);
var_dump(getopt('abc'));
var_dump(gc_collect_cycles());
?>
--EXPECTF--
array(2) {
  ["a"]=>
  bool(false)
  ["b"]=>
  bool(false)
}
array(2) {
  ["a"]=>
  bool(false)
  ["b"]=>
  bool(false)
}
int(0)

Warning: Array to string conversion in %s on line %d
array(2) {
  ["a"]=>
  bool(false)
  ["b"]=>
  bool(false)
}
int(2)
