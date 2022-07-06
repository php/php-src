--TEST--
Bug #60362: non-existent sub-sub keys should not have values
--FILE--
<?php
$arr = array('exists' => 'foz');

if (isset($arr['exists']['non_existent'])) {
    echo "sub-key 'non_existent' is set: ";
    var_dump($arr['exists']['non_existent']);
} else {
    echo "sub-key 'non_existent' is not set.\n";
}
if (isset($arr['exists'][1])) {
    echo "sub-key 1 is set: ";
    var_dump($arr['exists'][1]);
} else {
    echo "sub-key 1 is not set.\n";
}

echo "-------------------\n";
if (isset($arr['exists']['non_existent']['sub_sub'])) {
    echo "sub-key 'sub_sub' is set: ";
    var_dump($arr['exists']['non_existent']['sub_sub']);
} else {
    echo "sub-sub-key 'sub_sub' is not set.\n";
}
if (isset($arr['exists'][1][0])) {
    echo "sub-sub-key 0 is set: ";
    var_dump($arr['exists'][1][0]);
} else {
    echo "sub-sub-key 0 is not set.\n";
}

echo "-------------------\n";
if (empty($arr['exists']['non_existent'])) {
    echo "sub-key 'non_existent' is empty.\n";
} else {
    echo "sub-key 'non_existent' is not empty: ";
    var_dump($arr['exists']['non_existent']);
}
if (empty($arr['exists'][1])) {
    echo "sub-key 1 is empty.\n";
} else {
    echo "sub-key 1 is not empty: ";
    var_dump($arr['exists'][1]);
}

echo "-------------------\n";
if (empty($arr['exists']['non_existent']['sub_sub'])) {
    echo "sub-sub-key 'sub_sub' is empty.\n";
} else {
    echo "sub-sub-key 'sub_sub' is not empty: ";
    var_dump($arr['exists']['non_existent']['sub_sub']);
}
if (empty($arr['exists'][1][0])) {
    echo "sub-sub-key 0 is empty.\n";
} else {
    echo "sub-sub-key 0 is not empty: ";
    var_dump($arr['exists'][1][0]);
}
echo "DONE";
?>
--EXPECT--
sub-key 'non_existent' is not set.
sub-key 1 is set: string(1) "o"
-------------------
sub-sub-key 'sub_sub' is not set.
sub-sub-key 0 is set: string(1) "o"
-------------------
sub-key 'non_existent' is empty.
sub-key 1 is not empty: string(1) "o"
-------------------
sub-sub-key 'sub_sub' is empty.
sub-sub-key 0 is not empty: string(1) "o"
DONE
