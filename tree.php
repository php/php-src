<?php
$classTokens = token_get_all(
'<?php
class Test<T: Serializable, K> {
    public Serializable<T> $test;
    function __construct<T>() {}
}
');

var_export($classTokens);