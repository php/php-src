--TEST--
PREG_REPLACE_COUNT_CHANGES counts only effective changes
--EXTENSIONS--
pcre
--FILE--
<?php

function show($label, $before, $after, $count): void {
    if ($after === $before) {
        echo $label . ": NO REPLACEMENTS\n";
    } else {
        echo $label . ": CHANGED\n";
    }
    echo $label . ": " . $count . " REPLACEMENTS\n";
}

$before = "abca";

/* Default behavior, counts matches replaced even if identical */
$after = preg_replace('/a/', 'a', $before, -1, $count);
show("default", $before, $after, $count);

/* New behavior, counts only effective changes */
$after = preg_replace('/a/', 'a', $before, -1, $count, PREG_REPLACE_COUNT_CHANGES);
show("count_changes", $before, $after, $count);

/* Actual change */
$after = preg_replace('/a/', 'x', $before, -1, $count, PREG_REPLACE_COUNT_CHANGES);
show("count_changes_real", $before, $after, $count);
echo "count_changes_real: " . $after . "\n";

/* Backreference that reproduces original text */
$after = preg_replace('/(a)/', '$1', $before, -1, $count, PREG_REPLACE_COUNT_CHANGES);
show("backref_identity", $before, $after, $count);

/* Callback returning identical match */
$after = preg_replace_callback('/a/', fn($m) => $m[0], $before, -1, $count, PREG_REPLACE_COUNT_CHANGES);
show("callback_identity", $before, $after, $count);

/* Callback producing change */
$after = preg_replace_callback('/a/', fn($m) => 'x', $before, -1, $count, PREG_REPLACE_COUNT_CHANGES);
show("callback_change", $before, $after, $count);
echo "callback_change: " . $after . "\n";

/* Edge case: replacements change locally but cancel out globally */
function cancel_out_callback($arr) {
    return match ($arr[0]) {
        'a' => 'ab',
        'bba' => 'ba',
    };
}
$before3 = "abba";
$after = preg_replace_callback('/^a|bba/', 'cancel_out_callback', $before3, -1, $count, PREG_REPLACE_COUNT_CHANGES);
show("callback_cancel_out", $before3, $after, $count);

/* Empty string match behavior */
$before2 = "abc";
$after = preg_replace('/^/', '', $before2, -1, $count, PREG_REPLACE_COUNT_CHANGES);
show("empty_match_identity", $before2, $after, $count);

$after = preg_replace('/^/', 'Z', $before2, -1, $count, PREG_REPLACE_COUNT_CHANGES);
show("empty_match_change", $before2, $after, $count);
echo "empty_match_change: " . $after . "\n";

?>
--EXPECT--
default: NO REPLACEMENTS
default: 2 REPLACEMENTS
count_changes: NO REPLACEMENTS
count_changes: 0 REPLACEMENTS
count_changes_real: CHANGED
count_changes_real: 2 REPLACEMENTS
count_changes_real: xbcx
backref_identity: NO REPLACEMENTS
backref_identity: 0 REPLACEMENTS
callback_identity: NO REPLACEMENTS
callback_identity: 0 REPLACEMENTS
callback_change: CHANGED
callback_change: 2 REPLACEMENTS
callback_change: xbcx
callback_cancel_out: NO REPLACEMENTS
callback_cancel_out: 0 REPLACEMENTS
empty_match_identity: NO REPLACEMENTS
empty_match_identity: 0 REPLACEMENTS
empty_match_change: CHANGED
empty_match_change: 1 REPLACEMENTS
empty_match_change: Zabc
