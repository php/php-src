--TEST--
Test array(key: $value) alias for ['key' => $value]
--FILE--
<?php

call_user_func(function () {
    $values = array(
        key: 'other',
    );
    var_export(array(key: 'value'));
    echo "\n";
    list(key: $key) = $values;
    echo "key=$key\n";
});

// Reference syntax can be used and mixed
call_user_func(function () {
    $var = 123;
    $values = [key: [innerKey: &$var]];
    $values['key']['innerKey'] = 'updated';
    var_dump($var);
    // [key: [innerKey: &$otherVar]] would be equivalent and more consistent
    list('key' => list(innerKey: &$otherVar)) = $values;
    $var = 'updated again';
    var_dump($otherVar);
});

?>
--EXPECT--
array (
  'key' => 'value',
)
key=other
string(7) "updated"
string(13) "updated again"
