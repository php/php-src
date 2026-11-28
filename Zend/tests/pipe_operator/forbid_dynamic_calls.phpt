--TEST--
Pipe operator does not bypass forbid-dynamic-calls
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
            ['zz' => 1] |> extract(...);
        } catch (\Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        var_dump($zz ?? 'unset');

        try {
            ['zz' => 1] |> extract(?);
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
            ['zz' => 1] |> ex(...);
        } catch (\Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        var_dump($zz ?? 'unset');

        var_dump(42 |> extract(...));
    }
}

namespace ShadowNs {
    function extract($x) {
        return ['shadow' => $x];
    }

    function run(): void {
        var_dump([1] |> extract(...));
    }
}

namespace {
    function test_scope(): void {
        try {
            ['zz' => 1] |> extract(...);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        var_dump($zz ?? 'unset');

        try {
            ['zz' => 1] |> \extract(...);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            ['zz' => 1] |> EXTRACT(...);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            ['zz' => 1] |> ('extract')(...);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            ['zz' => 1] |> ('\extract')(...);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            ['zz' => 1] |> extract(?);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        $x = 5;
        try {
            'x' |> compact(...);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            'x' |> compact(?);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            0 |> func_get_args(?);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            0 |> func_num_args(?);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            0 |> get_defined_vars(?);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }

    function test_args($a, $b): void {
        try {
            0 |> func_get_arg(...);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }

        try {
            0 |> func_get_arg(?);
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
int(42)
array(1) {
  ["shadow"]=>
  array(1) {
    [0]=>
    int(1)
  }
}
