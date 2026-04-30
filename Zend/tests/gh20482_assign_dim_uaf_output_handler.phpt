--TEST--
GH-20482: heap use-after-free in ZEND_ASSIGN_DIM when an output handler clobbers the array during the undefined-variable warning
--FILE--
<?php
$a = [1, 2, 3];

// Chunk size 1 forces the handler to run synchronously when the warning
// emitted by the undefined RHS is buffered.
ob_start(function () use (&$a) {
    // Drop the only reference to the original array, freeing it.
    $a = "freed";
    return '';
}, 1);

// $undef is undefined: emitting the "Undefined variable" warning recurses
// into the output handler above, which frees $a. Without the fix the next
// store would write into the freed bucket.
$a[0] = $undef;

ob_end_clean();
var_dump($a);
echo "ok\n";
?>
--EXPECT--
string(5) "freed"
ok
