--TEST--
Bug #72305 (ReflectionParameter::isCallable is not implemented properly)
--FILE--
<?php
$callables = array(
    [['ArrayIterator', 'uasort'], 0],
    [['ArrayIterator', 'uksort'], 0],
    [['ArrayObject', 'uasort'], 0],
    [['ArrayObject', 'uksort'], 0],
    ['array_diff_uassoc', 2],
    ['array_diff_ukey', 2],
    ['array_filter', 1],
    ['array_intersect_uassoc', 2],
    ['array_intersect_ukey', 2],
    ['array_map', 0],
    ['array_reduce', 1],
    ['array_walk', 1],
    ['array_walk_recursive', 1],
    ['array_udiff', 2],
    ['array_udiff_assoc', 2],
    ['array_udiff_uassoc', 2],
    ['array_udiff_uassoc', 3],
    ['array_uintersect', 2],
    ['array_uintersect_assoc', 2],
    ['array_uintersect_uassoc', 2],
    ['array_uintersect_uassoc', 3],
    [['CallbackFilterIterator', '__construct'], 1],
    ['call_user_func', 0],
    ['call_user_func_array', 0],
    [['Closure', 'fromCallable'], 0],
    ['forward_static_call', 0],
    ['forward_static_call_array', 0],
    ['header_register_callback', 0],
    [['IntlChar', 'enumCharTypes'], 0],
    ['ldap_set_rebind_proc', 1],
    ['libxml_set_external_entity_loader', 0],
    ['mb_ereg_replace_callback', 1],
    [['Phar', 'webPhar'], 4],
    ['preg_replace_callback', 1],
    ['readline_callback_handler_install', 1],
    [['RecursiveCallbackFilterIterator', '__construct'], 1],
    ['register_shutdown_function', 0],
    ['set_error_handler', 0],
    ['set_exception_handler', 0],
    [['SQLite3', 'createAggregate'], 1],
    [['SQLite3', 'createAggregate'], 2],
    [['SQLite3', 'createCollation'], 1],
    [['SQLite3', 'createFunction'], 1],
    ['usort', 1],
    ['uasort', 1],
    ['uksort', 1],
);

foreach ($callables as list($func, $param)) {
    if (is_string($func) && function_exists($func)
        || is_array($func) && method_exists($func[0], $func[1])
    ) {
        $rp = new ReflectionParameter($func, $param);
        if (!$rp->isCallable()) {
            $name = is_string($func) ? $func : "$func[0]::$func[1]";
            echo "$name() parameter $param is not marked as callable\n";
        }
    }
}
?>
===DONE===
--EXPECT--
===DONE===
