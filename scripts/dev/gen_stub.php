#!/usr/bin/env php
<?php declare(strict_types=1);

use PhpParser\Node;
use PhpParser\Node\Stmt;

initPhpParser();

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

    public function __construct(
        string $name, array $args, ReturnInfo $return, int $numRequiredArgs
    ) {
        $this->name = $name;
        $this->args = $args;
        $this->return = $return;
        $this->numRequiredArgs = $numRequiredArgs;
    }
}

function parseFunction(Stmt\Function_ $func) : FuncInfo {
    $args = [];
    $numRequiredArgs = 0;
    foreach ($func->params as $i => $param) {
        $args[] = new ArgInfo(
            $param->var->name,
            $param->byRef,
            $param->variadic,
            $param->type ? Type::fromNode($param->type) : null
        );
        if (!$param->default) {
            $numRequiredArgs = $i + 1;
        }
    }

    $return = new ReturnInfo(
        $func->byRef,
        $func->returnType ? Type::fromNode($func->returnType) : null);
    return new FuncInfo($func->name->toString(), $args, $return, $numRequiredArgs);
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
    foreach ($stmts as $stmt) {
        if ($stmt instanceof Stmt\Nop) {
            continue;
        }

        if (!$stmt instanceof Stmt\Function_) {
            throw new Exception("Unexpected node {$stmt->getType()}");
        }

        // TODO Collect #if?
        $funcInfos[] = parseFunction($stmt);
    }

    return $funcInfos;
}

function funcInfoToCode(FuncInfo $funcInfo): string {
    $code = '';
    if ($funcInfo->return->type) {
        $returnType = $funcInfo->return->type;
        if ($returnType->isBuiltin) {
            $code .= sprintf(
                "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_%s, %d, %d, %s, %d)\n",
                $funcInfo->name, $funcInfo->return->byRef, $funcInfo->numRequiredArgs,
                $returnType->toTypeCode(), $returnType->isNullable
            );
        } else {
            throw new Exception("Not implemented");
        }
    } else {
        $code .= sprintf(
            "ZEND_BEGIN_ARG_INFO_EX(arginfo_%s, 0, %d, %d)\n",
            $funcInfo->name, $funcInfo->return->byRef, $funcInfo->numRequiredArgs
        );
    }

    foreach ($funcInfo->args as $argInfo) {
        if ($argInfo->isVariadic) {
            throw new Exception("Not implemented");
        }
        if ($argInfo->type) {
            if ($argInfo->type->isBuiltin) {
                $code .= sprintf(
                    "\tZEND_ARG_TYPE_INFO(%d, %s, %s, %d)\n",
                    $argInfo->byRef, $argInfo->name,
                    $argInfo->type->toTypeCode(), $argInfo->type->isNullable
                );
            } else {
                throw new Exception("Not implemented");
            }
        } else {
            $code .= sprintf("\tZEND_ARG_INFO(%d, %s)\n", $argInfo->byRef, $argInfo->name);
        }
    }

    $code .= "ZEND_END_ARG_INFO()";
    return $code;
}

/** @param FuncInfo[] $funcInfos */
function generateArginfoCode(array $funcInfos): string {
    return implode("\n\n", array_map('funcInfoToCode', $funcInfos));
}

function initPhpParser() {
    $version = "4.2.2";
    if (!is_dir(__DIR__ . "/PHP-Parser")) {
        $cwd = getcwd();
        chdir(__DIR__);
        passthru("wget https://github.com/nikic/PHP-Parser/archive/v$version.tar.gz");
        passthru("mkdir PHP-Parser");
        passthru("tar xvzf v$version.tar.gz -C PHP-Parser --strip-components 1");
        passthru("rm v$version.tar.gz");
        chdir($cwd);
    }

    spl_autoload_register(function(string $class) {
        if (strpos($class, "PhpParser\\") == 0) {
            $fileName = __DIR__ . "/PHP-Parser/lib/" . str_replace("\\", "/", $class) . ".php";
            require $fileName;
        }
    });
}
