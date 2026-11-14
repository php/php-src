--TEST--
GH-25449 (Fetch on a by-reference call result passed as a by-value argument)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php
function &registry(): array {
    static $r = ['k' => 'func'];
    return $r;
}

class K {
    public $p = ['k' => 'prop'];

    public static function &staticRef(): array {
        static $r = ['k' => 'static'];
        return $r;
    }

    public function &methodRef(): array {
        return $this->p;
    }

    public function &self(): K {
        return $this;
    }
}

function dim(): string { return consume(registry()['k']); }
function staticDim(): string { return consume(K::staticRef()['k']); }
function methodDim(K $o): string { return consume($o->methodRef()['k']); }
function objDim(K $o): string { return consume($o->self()->p['k']); }
function chainedDim(): string { return consume(registry()['k'][0]); }
function closureDim(Closure $c): string { return consume($c()['k']); }
function assignRefDim(): string { $x = ['k' => 'assign-ref']; return consume(($y = &$x)['k']); }
function assignRefObj(): string { $x = new K; return consume(($y = &$x)->p['k']); }
function listRefDim(): string { $a = [['k' => 'list-ref']]; return consume(([&$z] = $a)[0]['k']); }

function consume(string $s): string { return $s; }

$o = new K;
echo "dim: ", dim(), "\n";
echo "static: ", staticDim(), "\n";
echo "method: ", methodDim($o), "\n";
echo "obj: ", objDim($o), "\n";
echo "chained: ", chainedDim(), "\n";
echo "closure: ", closureDim(function &(): array { return registry(); }), "\n";
echo "assign-ref: ", assignRefDim(), "\n";
echo "assign-ref obj: ", assignRefObj(), "\n";
echo "list-ref: ", listRefDim(), "\n";
?>
--EXPECT--
dim: func
static: static
method: prop
obj: prop
chained: f
closure: func
assign-ref: assign-ref
assign-ref obj: prop
list-ref: list-ref
