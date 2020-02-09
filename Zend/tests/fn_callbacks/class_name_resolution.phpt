--TEST--
fn:: callback with static classname
--FILE--
<?php
// Keep in sync with class_name_as_scalar.phpt

namespace Foo\Bar {
    class One {
    }
    class Two extends One {
        public static function run() {
            var_dump((fn::self::meth)[0]); // self compile time lookup
            var_dump((fn::static::meth)[0]); // runtime lookup
            var_dump((fn::parent::meth)[0]); // runtime lookup
            var_dump((fn::Baz::meth)[0]); // default compile time lookup
        }
    }
    class Three extends Two {
    }
    echo "In NS\n";
    var_dump((fn::Moo::meth)[0]); // resolve in namespace
}

namespace {
    use Bee\Bop as Moo,
        Foo\Bar\One;
    echo "Top\n";
    var_dump((fn::One::meth)[0]); // resolve from use
    var_dump((fn::Boo::meth)[0]); // resolve in global namespace
    var_dump((fn::Moo::meth)[0]); // resolve from use as
    var_dump((fn::\Moo::meth)[0]); // resolve fully qualified
    Foo\Bar\Two::run(); // resolve runtime lookups
    echo "Parent\n";
    Foo\Bar\Three::run(); // resolve runtime lookups with inheritance
}

?>
--EXPECT--
In NS
string(11) "Foo\Bar\Moo"
Top
string(11) "Foo\Bar\One"
string(3) "Boo"
string(7) "Bee\Bop"
string(3) "Moo"
string(11) "Foo\Bar\Two"
string(11) "Foo\Bar\Two"
string(11) "Foo\Bar\One"
string(11) "Foo\Bar\Baz"
Parent
string(11) "Foo\Bar\Two"
string(13) "Foo\Bar\Three"
string(11) "Foo\Bar\One"
string(11) "Foo\Bar\Baz"
