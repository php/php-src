--TEST--
GH-20482: ZEND_ASSIGN_DIM does not crash when the output handler mutates the array via a reference during the undefined-variable warning
--FILE--
<?php
$a = ['existing' => 0];

ob_start(function () use (&$a) {
    // Mutate the array many times via the reference. Each nested
    // ZEND_ASSIGN_DIM observes the temporary refcount bump from the outer
    // handler and SEPARATEs the array, so the writes land in a duplicate;
    // the outer assignment is then aborted cleanly via assign_dim_error
    // when the original drops to refcount 0. The point of the test is
    // simply that the whole thing must not UAF or crash, regardless of
    // which array $a ends up pointing at.
    for ($i = 0; $i < 64; $i++) {
        $a["k$i"] = $i;
    }
    return '';
}, 1);

$a['target'] = $undef;

ob_end_clean();
var_dump(count($a) >= 65);
echo "ok\n";
?>
--EXPECT--
bool(true)
ok
