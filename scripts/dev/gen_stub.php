#!/usr/bin/env php
<?php declare(strict_types=1);

use PhpParser\Node;
use PhpParser\Node\Stmt;

error_reporting(E_ALL);

try {
    initPhpParser();
} catch (Exception $e) {
    echo "{$e->getMessage()}\n";
    exit(1);
}

if ($argc < 2) {
    die("Usage: php gen_stub.php foobar.stub.php\n");
}

$stubFile = $argv[1];
$arginfoFile = str_replace('.stub.php', '', $stubFile) . '_arginfo.h';

try {
    $funcInfos = parseStubFile($stubFile);
    $arginfoCode = generateArgInfoCode($funcInfos);
    file_put_contents($arginfoFile, $arginfoCode);
} catch (Exception $e) {
    echo "Caught {$e->getMessage()} while processing $stubFile\n";
    exit(1);
}

class Type {
    /** @var string */
    public $name;
    /** @var bool */
    public $isBuiltin;
    /** @var bool */
    public $isNullable;

    public function __construct(string $name, bool $isBuiltin, bool $isNullable = false) {
        $this->name = $name;
        $this->isBuiltin = $isBuiltin;
        $this->isNullable = $isNullable;
    }

    public static function fromNode(Node $node) {
        if ($node instanceof Node\NullableType) {
            $type = self::fromNode($node->type);
            return new Type($type->name, $type->isBuiltin, true);
        }
        if ($node instanceof Node\Name) {
            assert($node->isFullyQualified());
            return new Type($node->toString(), false);
        }
        if ($node instanceof Node\Identifier) {
            return new Type($node->toString(), true);
        }
        throw new Exception("Unexpected node type");
    }

    public function toTypeCode() {
        assert($this->isBuiltin);
        switch (strtolower($this->name)) {
        case "bool":
            return "_IS_BOOL";
        case "int":
            return "IS_LONG";
        case "float":
            return "IS_DOUBLE";
        case "string":
            return "IS_STRING";
        case "array":
            return "IS_ARRAY";
        case "object":
            return "IS_OBJECT";
        case "void":
            return "IS_VOID";
        default:
            throw new Exception("Not implemented: $this->name");
        }
    }

    public static function equals(?Type $a, ?Type $b): bool {
        if ($a === null || $b === null) {
            return $a === $b;
        }

        return $a->name === $b->name
            && $a->isBuiltin === $b->isBuiltin
            && $a->isNullable === $b->isNullable;
    }
}

class ArgInfo {
    /** @var string */
    public $name;
    /** @var bool */
    public $byRef;
    /** @var bool */
    public $isVariadic;
    /** @var Type|null */
    public $type;

    public function __construct(string $name, bool $byRef, bool $isVariadic, ?Type $type) {
        $this->name = $name;
        $this->byRef = $byRef;
        $this->isVariadic = $isVariadic;
        $this->type = $type;
    }

    public function equals(ArgInfo $other): bool {
        return $this->name === $other->name
            && $this->byRef === $other->byRef
            && $this->isVariadic === $other->isVariadic
            && Type::equals($this->type, $other->type);
    }
}

class ReturnInfo {
    /** @var bool */
    public $byRef;
    /** @var Type|null */
    public $type;

    public function __construct(bool $byRef, ?Type $type) {
        $this->byRef = $byRef;
        $this->type = $type;
    }

    public function equals(ReturnInfo $other): bool {
        return $this->byRef === $other->byRef
            && Type::equals($this->type, $other->type);
    }
}

class FuncInfo {
    /** @var string */
    public $name;
    /** @var ArgInfo[] */
    public $args;
    /** @var ReturnInfo */
    public $return;
    /** @var int */
    public $numRequiredArgs;
    /** @var string|null */
    public $cond;

    public function __construct(
        string $name, array $args, ReturnInfo $return, int $numRequiredArgs, ?string $cond
    ) {
        $this->name = $name;
        $this->args = $args;
        $this->return = $return;
        $this->numRequiredArgs = $numRequiredArgs;
        $this->cond = $cond;
    }

    public function equalsApartFromName(FuncInfo $other): bool {
        if (count($this->args) !== count($other->args)) {
            return false;
        }

        for ($i = 0; $i < count($this->args); $i++) {
            if (!$this->args[$i]->equals($other->args[$i])) {
                return false;
            }
        }

        return $this->return->equals($other->return)
            && $this->numRequiredArgs === $other->numRequiredArgs
            && $this->cond === $other->cond;
    }
}

function parseFunctionLike(string $name, Node\FunctionLike $func, ?string $cond): FuncInfo {
    $args = [];
    $numRequiredArgs = 0;
    foreach ($func->getParams() as $i => $param) {
        $args[] = new ArgInfo(
            $param->var->name,
            $param->byRef,
            $param->variadic,
            $param->type ? Type::fromNode($param->type) : null
        );
        if (!$param->default && !$param->variadic) {
            $numRequiredArgs = $i + 1;
        }
    }

    $returnType = $func->getReturnType();
    $return = new ReturnInfo(
        $func->returnsByRef(),
        $returnType ? Type::fromNode($returnType) : null);
    return new FuncInfo($name, $args, $return, $numRequiredArgs, $cond);
}

function parseClass(Stmt\Class_ $class): ClassInfo {
    $funcs = [];
    $className = $class->name->toString();
    foreach ($class as $stmt) {
        if (!$stmt instanceof Stmt\ClassMethod) {
            throw new Exception("Not implemented class statement");
        }

        $funcs[] = parseFunctionLike($className . '_' . $stmt->name->toString(), $stmt);
    }
    return new ClassInfo($className, $funcs);
}

/** @return FuncInfo[] */
function parseStubFile(string $fileName) {
    if (!file_exists($fileName)) {
        throw new Exception("File $fileName does not exist");
    }

    $code = file_get_contents($fileName);

    $lexer = new PhpParser\Lexer();
    $parser = new PhpParser\Parser\Php7($lexer);
    $nodeTraverser = new PhpParser\NodeTraverser;
    $nodeTraverser->addVisitor(new PhpParser\NodeVisitor\NameResolver);

    $stmts = $parser->parse($code);
    $nodeTraverser->traverse($stmts);

    $funcInfos = [];
    $cond = null;
    foreach ($stmts as $stmt) {
        foreach ($stmt->getComments() as $comment) {
            $text = trim($comment->getText());
            if (preg_match('/^#if\s+(.+)$/', $text, $matches)) {
                if ($cond !== null) {
                    throw new Exception("Not implemented preprocessor directive");
                }
                $cond = $matches[1];
            } else if ($text === '#endif') {
                $cond = null;
            }
        }

        if ($stmt instanceof Stmt\Nop) {
            continue;
        }

        if ($stmt instanceof Stmt\Function_) {
            $funcInfos[] = parseFunctionLike($stmt->name->toString(), $stmt, $cond);
            continue;
        }

        if ($stmt instanceof Stmt\ClassLike) {
            $className = $stmt->name->toString();
            foreach ($stmt->stmts as $classStmt) {
                if ($classStmt instanceof Stmt\Nop) {
                    continue;
                }

                if (!$classStmt instanceof Stmt\ClassMethod) {
                    throw new Exception("Not implemented {$classStmt->getType()}");
                }

                $funcInfos[] = parseFunctionLike(
                    $className . '_' . $classStmt->name->toString(), $classStmt, $cond);
            }
            continue;
        }

        throw new Exception("Unexpected node {$stmt->getType()}");
    }

    return $funcInfos;
}

function funcInfoToCode(FuncInfo $funcInfo): string {
    $code = '';
    if ($funcInfo->cond) {
        $code .= "#if {$funcInfo->cond}\n";
    }
    if ($funcInfo->return->type) {
        $returnType = $funcInfo->return->type;
        if ($returnType->isBuiltin) {
            $code .= sprintf(
                "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_%s, %d, %d, %s, %d)\n",
                $funcInfo->name, $funcInfo->return->byRef, $funcInfo->numRequiredArgs,
                $returnType->toTypeCode(), $returnType->isNullable
            );
        } else {
            $code .= sprintf(
                "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_%s, %d, %d, %s, %d)\n",
                $funcInfo->name, $funcInfo->return->byRef, $funcInfo->numRequiredArgs,
                $returnType->name, $returnType->isNullable
            );
        }
    } else {
        $code .= sprintf(
            "ZEND_BEGIN_ARG_INFO_EX(arginfo_%s, 0, %d, %d)\n",
            $funcInfo->name, $funcInfo->return->byRef, $funcInfo->numRequiredArgs
        );
    }

    foreach ($funcInfo->args as $argInfo) {
        if ($argInfo->isVariadic) {
            if ($argInfo->type) {
                throw new Exception("Not implemented");
            }
            $code .= sprintf(
                "\tZEND_ARG_VARIADIC_INFO(%d, %s)\n",
                $argInfo->byRef, $argInfo->name
            );
        } else if ($argInfo->type) {
            if ($argInfo->type->isBuiltin) {
                $code .= sprintf(
                    "\tZEND_ARG_TYPE_INFO(%d, %s, %s, %d)\n",
                    $argInfo->byRef, $argInfo->name,
                    $argInfo->type->toTypeCode(), $argInfo->type->isNullable
                );
            } else {
                $code .= sprintf(
                    "\tZEND_ARG_OBJ_INFO(%d, %s, %s, %d)\n",
                    $argInfo->byRef, $argInfo->name,
                    $argInfo->type->name, $argInfo->type->isNullable
                );
            }
        } else {
            $code .= sprintf("\tZEND_ARG_INFO(%d, %s)\n", $argInfo->byRef, $argInfo->name);
        }
    }

    $code .= "ZEND_END_ARG_INFO()";
    if ($funcInfo->cond) {
        $code .= "\n#endif";
    }
    return $code;
}

/** @param FuncInfo[] $funcInfos */
function generateArginfoCode(array $funcInfos): string {
    $code = "/* This is a generated file, edit the .stub.php file instead. */";
    $generatedFuncInfos = [];
    foreach ($funcInfos as $funcInfo) {
        /* If there already is an equivalent arginfo structure, only emit a #define */
        foreach ($generatedFuncInfos as $generatedFuncInfo) {
            if ($generatedFuncInfo->equalsApartFromName($funcInfo)) {
                $code .= sprintf(
                    "\n\n#define arginfo_%s arginfo_%s",
                    $funcInfo->name, $generatedFuncInfo->name
                );
                continue 2;
            }
        }

        $code .= "\n\n" . funcInfoToCode($funcInfo);
        $generatedFuncInfos[] = $funcInfo;
    }
    return $code . "\n";
}

function initPhpParser() {
    $version = "4.2.2";
    $phpParserDir = __DIR__ . "/PHP-Parser-$version";
    if (!is_dir($phpParserDir)) {
        $cwd = getcwd();
        chdir(__DIR__);
        passthru("wget https://github.com/nikic/PHP-Parser/archive/v$version.tar.gz", $exit);
        if ($exit !== 0) {
            throw new Exception("Failed to download PHP-Parser tarball");
        }
        if (!mkdir($phpParserDir)) {
            throw new Exception("Failed to create directory $phpParserDir");
        }
        passthru("tar xvzf v$version.tar.gz -C PHP-Parser-$version --strip-components 1", $exit);
        if ($exit !== 0) {
            throw new Exception("Failed to extract PHP-Parser tarball");
        }
        unlink(__DIR__ . "/v$version.tar.gz");
        chdir($cwd);
    }

    spl_autoload_register(function(string $class) use($phpParserDir) {
        if (strpos($class, "PhpParser\\") === 0) {
            $fileName = $phpParserDir . "/lib/" . str_replace("\\", "/", $class) . ".php";
            require $fileName;
        }
    });
}
