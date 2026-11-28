--TEST--
array_map(): foreach optimization does not bypass forbid-dynamic-calls
--FILE--
<?php

namespace Other {
    function identity($x) {
        return $x;
    }
}

namespace FallbackNs {
    function run(): void {
        try {
            \array_map(extract(...), [['zz' => 1]]);
        } catch (\Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        var_dump($zz ?? 'unset');

        try {
            \array_map(extract(?), []);
        } catch (\Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }
}

namespace AliasNs {
    use function extract as ex;
    use function Other\identity as extract;

    function run(): void {
        try {
            \array_map(ex(...), [['zz' => 1]]);
        } catch (\Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        var_dump($zz ?? 'unset');

        var_dump(\array_map(extract(...), [1, 2]));
    }
}

namespace ShadowNs {
    function extract($x) {
        return ['shadow' => $x];
    }

    function run(): void {
        var_dump(\array_map(extract(...), [[1]]));
    }
}

namespace {
    function test_scope(): void {
        try {
            array_map(extract(...), [['zz' => 1]]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        var_dump($zz ?? 'unset');

        try {
            array_map(\extract(...), [['zz' => 1]]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            array_map(EXTRACT(...), [['zz' => 1]]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            array_map(('extract')(...), [['zz' => 1]]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            array_map(('\extract')(...), [['zz' => 1]]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            array_map(extract(?), [['zz' => 1]]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            array_map(extract(?), []);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        var_dump(array_map(extract(...), []));

        $x = 5;
        try {
            array_map(compact(...), ['x']);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            array_map(compact(?), ['x']);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            array_map(func_get_args(?), []);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            array_map(func_num_args(?), []);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            array_map(get_defined_vars(?), []);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }

    function test_args($a, $b): void {
        try {
            array_map(func_get_arg(...), [1, 0]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            array_map(func_get_arg(?), [1, 0]);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }

    test_scope();
    test_args('A', 'B');
    FallbackNs\run();
    AliasNs\run();
    ShadowNs\run();
}

?>
--EXPECT--
Error: Cannot call extract() dynamically
string(5) "unset"
Error: Cannot call extract() dynamically
Error: Cannot call extract() dynamically
Error: Cannot call extract() dynamically
Error: Cannot call extract() dynamically
Error: Cannot call extract() dynamically
Error: Cannot call extract() dynamically
array(0) {
}
Error: Cannot call compact() dynamically
Error: Cannot call compact() dynamically
Error: Cannot call func_get_args() dynamically
Error: Cannot call func_num_args() dynamically
Error: Cannot call get_defined_vars() dynamically
Error: Cannot call func_get_arg() dynamically
Error: Cannot call func_get_arg() dynamically
Error: Cannot call extract() dynamically
string(5) "unset"
Error: Cannot call extract() dynamically
Error: Cannot call extract() dynamically
string(5) "unset"
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(1) {
  [0]=>
  array(1) {
    ["shadow"]=>
    array(1) {
      [0]=>
      int(1)
    }
  }
}
