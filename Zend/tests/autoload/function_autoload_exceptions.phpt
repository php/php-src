--TEST--
Function autoloading: loader exceptions propagate, failures are not cached
--FILE--
<?php
echo "== direct propagates ==\n";
$thrower = function (string $name) {
    throw new RuntimeException("Autoload failed for: $name");
};
spl_autoload_register_function_loader($thrower);
try {
    missing_func();
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
// failed autoloads are not cached, so the loader runs again
try {
    missing_func();
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}

echo "== dynamic propagates ==\n";
$f = 'missing_func';
try {
    $f();
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
spl_autoload_unregister_function_loader($thrower);

echo "== callable resolution wrapping ==\n";
// is_callable() rethrows the loader's exception directly; the call/closure APIs
// wrap it in a TypeError but keep it as the previous exception. Either way the
// loader's exception must not be swallowed.
$thrower = function (string $name) {
    throw new RuntimeException("loader failed");
};
spl_autoload_register_function_loader($thrower);
function check(callable $cb): void {
    try {
        $cb();
        echo "no exception\n";
    } catch (\Throwable $e) {
        $origin = $e instanceof RuntimeException ? $e : $e->getPrevious();
        echo get_class($e), " -> ", get_class($origin), ": ", $origin->getMessage(), "\n";
    }
}
check(fn() => is_callable('boom'));
check(fn() => call_user_func('boom'));
check(fn() => Closure::fromCallable('boom'));
spl_autoload_unregister_function_loader($thrower);

echo "== silent decline retries ==\n";
// A name that fails to load is retried; there is no negative cache.
// A loader that silently declines (no throw, no definition) does not poison the
// name, so a name unloadable now may become loadable later. (The throwing-loader
// retry is covered by the "direct propagates" section above.)
$attempts = 0;
$enabled = false;
$loader = function (string $name) use (&$attempts, &$enabled) {
    if ($name !== 'late_func') {
        return;
    }
    $attempts++;
    echo "attempt $attempts\n";
    if ($enabled) {
        eval('function late_func() { return "loaded on retry"; }');
    }
    // otherwise decline silently: no exception, no definition
};
spl_autoload_register_function_loader($loader);
try {
    late_func();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    late_func();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
$enabled = true;
var_dump(late_func());
echo "attempts: $attempts\n";
spl_autoload_unregister_function_loader($loader);
?>
--EXPECT--
== direct propagates ==
Autoload failed for: missing_func
Autoload failed for: missing_func
== dynamic propagates ==
Autoload failed for: missing_func
== callable resolution wrapping ==
RuntimeException -> RuntimeException: loader failed
TypeError -> RuntimeException: loader failed
TypeError -> RuntimeException: loader failed
== silent decline retries ==
attempt 1
Call to undefined function late_func()
attempt 2
Call to undefined function late_func()
attempt 3
string(15) "loaded on retry"
attempts: 3
