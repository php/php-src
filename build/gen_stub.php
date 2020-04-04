#!/usr/bin/env php
<?php declare(strict_types=1);

use PhpParser\Comment\Doc as DocComment;
use PhpParser\Node;
use PhpParser\Node\Expr;
use PhpParser\Node\Stmt;

error_reporting(E_ALL);

try {
    initPhpParser();
} catch (Exception $e) {
    echo "{$e->getMessage()}\n";
    exit(1);
}

if ($argc >= 2) {
    if (is_file($argv[1])) {
        // Generate single file.
        processStubFile($argv[1]);
    } else if (is_dir($argv[1])) {
        processDirectory($argv[1]);
    } else {
        echo "$argv[1] is neither a file nor a directory.\n";
        exit(1);
    }
} else {
    // Regenerate all stub files we can find.
    processDirectory('.');
}

function processDirectory(string $dir) {
    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir),
        RecursiveIteratorIterator::LEAVES_ONLY
    );
    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (preg_match('/\.stub\.php$/', $pathName)) {
            processStubFile($pathName);
        }
    }
}

function processStubFile(string $stubFile) {
    $arginfoFile = str_replace('.stub.php', '', $stubFile) . '_arginfo.h';

    try {
        $fileInfo = parseStubFile($stubFile);
        $arginfoCode = generateArgInfoCode($fileInfo);
        file_put_contents($arginfoFile, $arginfoCode);
    } catch (Exception $e) {
        echo "In $stubFile:\n{$e->getMessage()}\n";
        exit(1);
    }
}

class SimpleType {
    /** @var string */
    public $name;
    /** @var bool */
    public $isBuiltin;

    public function __construct(string $name, bool $isBuiltin) {
        $this->name = $name;
        $this->isBuiltin = $isBuiltin;
    }

    public static function fromNode(Node $node) {
        if ($node instanceof Node\Name) {
            assert($node->isFullyQualified());
            return new SimpleType($node->toString(), false);
        }
        if ($node instanceof Node\Identifier) {
            return new SimpleType($node->toString(), true);
        }
        throw new Exception("Unexpected node type");
    }

    public function isNull() {
        return $this->isBuiltin && $this->name === 'null';
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
        case "callable":
            return "IS_CALLABLE";
        default:
            throw new Exception("Not implemented: $this->name");
        }
    }

    public function toTypeMask() {
        assert($this->isBuiltin);
        switch (strtolower($this->name)) {
        case "null":
            return "MAY_BE_NULL";
        case "false":
            return "MAY_BE_FALSE";
        case "bool":
            return "MAY_BE_BOOL";
        case "int":
            return "MAY_BE_LONG";
        case "float":
            return "MAY_BE_DOUBLE";
        case "string":
            return "MAY_BE_STRING";
        case "array":
            return "MAY_BE_ARRAY";
        case "object":
            return "MAY_BE_OBJECT";
        case "callable":
            return "MAY_BE_CALLABLE";
        default:
            throw new Exception("Not implemented: $this->name");
        }
    }

    public function toEscapedName(): string {
        return str_replace('\\', '\\\\', $this->name);
    }

    public function equals(SimpleType $other) {
        return $this->name === $other->name
            && $this->isBuiltin === $other->isBuiltin;
    }
}

class Type {
    /** @var SimpleType[] $types */
    public $types;

    public function __construct(array $types) {
        $this->types = $types;
    }

    public static function fromNode(Node $node) {
        if ($node instanceof Node\UnionType) {
            return new Type(array_map(['SimpleType', 'fromNode'], $node->types));
        }
        if ($node instanceof Node\NullableType) {
            return new Type([
                SimpleType::fromNode($node->type),
                new SimpleType('null', true),
            ]);
        }
        return new Type([SimpleType::fromNode($node)]);
    }

    public function isNullable(): bool {
        foreach ($this->types as $type) {
            if ($type->isNull()) {
                return true;
            }
        }
        return false;
    }

    public function getWithoutNull(): Type {
        return new Type(array_filter($this->types, function(SimpleType $type) {
            return !$type->isNull();
        }));
    }

    public function tryToSimpleType(): ?SimpleType {
        $withoutNull = $this->getWithoutNull();
        if (count($withoutNull->types) === 1) {
            return $withoutNull->types[0];
        }
        return null;
    }

    public function tryToRepresentableType(): ?RepresentableType {
        $classType = null;
        $builtinTypes = [];
        foreach ($this->types as $type) {
            if ($type->isBuiltin) {
                $builtinTypes[] = $type;
            } else if ($classType === null) {
                $classType = $type;
            } else {
                // We can only represent a single class type.
                return false;
            }
        }
        return new RepresentableType($classType, $builtinTypes);
    }

    public static function equals(?Type $a, ?Type $b): bool {
        if ($a === null || $b === null) {
            return $a === $b;
        }

        if (count($a->types) !== count($b->types)) {
            return false;
        }

        for ($i = 0; $i < count($a->types); $i++) {
            if (!$a->types[$i]->equals($b->types[$i])) {
                return false;
            }
        }

        return true;
    }
}

class RepresentableType {
    /** @var ?SimpleType $classType */
    public $classType;
    /** @var SimpleType[] $builtinTypes */
    public $builtinTypes;

    public function __construct(?SimpleType $classType, array $builtinTypes) {
        $this->classType = $classType;
        $this->builtinTypes = $builtinTypes;
    }

    public function toTypeMask(): string {
        return implode('|', array_map(function(SimpleType $type) {
            return $type->toTypeMask();
        }, $this->builtinTypes));
    }
}

class ArgInfo {
    const SEND_BY_VAL = 0;
    const SEND_BY_REF = 1;
    const SEND_PREFER_REF = 2;

    /** @var string */
    public $name;
    /** @var int */
    public $sendBy;
    /** @var bool */
    public $isVariadic;
    /** @var Type|null */
    public $type;

    public function __construct(string $name, int $sendBy, bool $isVariadic, ?Type $type) {
        $this->name = $name;
        $this->sendBy = $sendBy;
        $this->isVariadic = $isVariadic;
        $this->type = $type;
    }

    public function equals(ArgInfo $other): bool {
        return $this->name === $other->name
            && $this->sendBy === $other->sendBy
            && $this->isVariadic === $other->isVariadic
            && Type::equals($this->type, $other->type);
    }

    public function getSendByString(): string {
        switch ($this->sendBy) {
        case self::SEND_BY_VAL:
            return "0";
        case self::SEND_BY_REF:
            return "1";
        case self::SEND_PREFER_REF:
            return "ZEND_SEND_PREFER_REF";
        }
        throw new Exception("Invalid sendBy value");
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
    /** @var ?string */
    public $className;
    /** @var ?string */
    public $alias;
    /** @var ArgInfo[] */
    public $args;
    /** @var ReturnInfo */
    public $return;
    /** @var int */
    public $numRequiredArgs;
    /** @var string|null */
    public $cond;

    public function __construct(
        string $name, ?string $className, ?string $alias, array $args, ReturnInfo $return,
        int $numRequiredArgs, ?string $cond
    ) {
        $this->name = $name;
        $this->className = $className;
        $this->alias = $alias;
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

    public function getArgInfoName(): string {
        if ($this->className) {
            return 'arginfo_class_' . $this->className . '_' . $this->name;
        }
        return 'arginfo_' . $this->name;
    }
}

class ClassInfo {
    /** @var string */
    public $name;
    /** @var FuncInfo[] */
    public $funcInfos;

    public function __construct(string $name, array $funcInfos) {
        $this->name = $name;
        $this->funcInfos = $funcInfos;
    }
}

class FileInfo {
    /** @var FuncInfo[] */
    public $funcInfos;
    /** @var ClassInfo[] */
    public $classInfos;
    /** @var bool */
    public $generateFunctionEntries;

    public function __construct(
            array $funcInfos, array $classInfos, bool $generateFunctionEntries) {
        $this->funcInfos = $funcInfos;
        $this->classInfos = $classInfos;
        $this->generateFunctionEntries = $generateFunctionEntries;
    }

    public function getAllFuncInfos(): iterable {
        yield from $this->funcInfos;
        foreach ($this->classInfos as $classInfo) {
            yield from $classInfo->funcInfos;
        }
    }
}

class DocCommentTag {
    /** @var string */
    public $name;
    /** @var ?string */
    public $value;

    public function __construct(string $name, ?string $value) {
        $this->name = $name;
        $this->value = $value;
    }

    public function getValue(): string {
        if ($this->value === null) {
            throw new Exception("@$this->name does not have a value");
        }

        return $this->value;
    }

    public function getVariableName(): string {
        $value = $this->getValue();
        if ($value === null || strlen($value) === 0 || $value[0] !== '$') {
            throw new Exception("@$this->name not followed by variable name");
        }

        return substr($value, 1);
    }
}

/** @return DocCommentTag[] */
function parseDocComment(DocComment $comment): array {
    $commentText = substr($comment->getText(), 2, -2);
    $tags = [];
    foreach (explode("\n", $commentText) as $commentLine) {
        $regex = '/^\*\s*@([a-z-]+)(?:\s+(.+))$/';
        if (preg_match($regex, trim($commentLine), $matches, PREG_UNMATCHED_AS_NULL)) {
            $tags[] = new DocCommentTag($matches[1], $matches[2]);
        }
    }

    return $tags;
}

function parseFunctionLike(
    string $name, ?string $className, Node\FunctionLike $func, ?string $cond
): FuncInfo {
    $comment = $func->getDocComment();
    $paramMeta = [];
    $alias = null;
    $haveDocReturnType = false;

    if ($comment) {
        $tags = parseDocComment($comment);
        foreach ($tags as $tag) {
            if ($tag->name === 'prefer-ref') {
                $varName = $tag->getVariableName();
                if (!isset($paramMeta[$varName])) {
                    $paramMeta[$varName] = [];
                }
                $paramMeta[$varName]['preferRef'] = true;
            } else if ($tag->name === 'alias') {
                $alias = $tag->getValue();
            } else if ($tag->name === 'return') {
                $haveDocReturnType = true;
            }
        }
    }

    $args = [];
    $numRequiredArgs = 0;
    $foundVariadic = false;
    foreach ($func->getParams() as $i => $param) {
        $varName = $param->var->name;
        $preferRef = !empty($paramMeta[$varName]['preferRef']);
        unset($paramMeta[$varName]);

        if ($preferRef) {
            $sendBy = ArgInfo::SEND_PREFER_REF;
        } else if ($param->byRef) {
            $sendBy = ArgInfo::SEND_BY_REF;
        } else {
            $sendBy = ArgInfo::SEND_BY_VAL;
        }

        if ($foundVariadic) {
            throw new Exception("Error in function $name: only the last parameter can be variadic");
        }

        $type = $param->type ? Type::fromNode($param->type) : null;
        if ($param->default instanceof Expr\ConstFetch &&
            $param->default->name->toLowerString() === "null" &&
            $type && !$type->isNullable()
        ) {
            throw new Exception(
                "Parameter $varName of function $name has null default, but is not nullable");
        }

        $foundVariadic = $param->variadic;

        $args[] = new ArgInfo(
            $varName,
            $sendBy,
            $param->variadic,
            $type
        );
        if (!$param->default && !$param->variadic) {
            $numRequiredArgs = $i + 1;
        }
    }

    foreach (array_keys($paramMeta) as $var) {
        throw new Exception("Found metadata for invalid param $var of function $name");
    }

    $returnType = $func->getReturnType();
    if ($returnType === null && !$haveDocReturnType && substr($name, 0, 2) !== '__') {
        throw new Exception("Missing return type for function $name()");
    }

    $return = new ReturnInfo(
        $func->returnsByRef(),
        $returnType ? Type::fromNode($returnType) : null);
    return new FuncInfo($name, $className, $alias, $args, $return, $numRequiredArgs, $cond);
}

function handlePreprocessorConditions(array &$conds, Stmt $stmt): ?string {
    foreach ($stmt->getComments() as $comment) {
        $text = trim($comment->getText());
        if (preg_match('/^#\s*if\s+(.+)$/', $text, $matches)) {
            $conds[] = $matches[1];
        } else if (preg_match('/^#\s*ifdef\s+(.+)$/', $text, $matches)) {
            $conds[] = "defined($matches[1])";
        } else if (preg_match('/^#\s*ifndef\s+(.+)$/', $text, $matches)) {
            $conds[] = "!defined($matches[1])";
        } else if (preg_match('/^#\s*else$/', $text)) {
            if (empty($conds)) {
                throw new Exception("Encountered else without corresponding #if");
            }
            $cond = array_pop($conds);
            $conds[] = "!($cond)";
        } else if (preg_match('/^#\s*endif$/', $text)) {
            if (empty($conds)) {
                throw new Exception("Encountered #endif without corresponding #if");
            }
            array_pop($conds);
        } else if ($text[0] === '#') {
            throw new Exception("Unrecognized preprocessor directive \"$text\"");
        }
    }

    return empty($conds) ? null : implode(' && ', $conds);
}

function getFileDocComment(array $stmts): ?DocComment {
    if (empty($stmts)) {
        return null;
    }

    $comments = $stmts[0]->getComments();
    if (empty($comments)) {
        return null;
    }

    if ($comments[0] instanceof DocComment) {
        return $comments[0];
    }

    return null;
}

function parseStubFile(string $fileName): FileInfo {
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

    $generateFunctionEntries = false;
    $fileDocComment = getFileDocComment($stmts);
    if ($fileDocComment) {
        if (strpos($fileDocComment->getText(), '@generate-function-entries') !== false) {
            $generateFunctionEntries = true;
        }
    }

    $funcInfos = [];
    $classInfos = [];
    $conds = [];
    foreach ($stmts as $stmt) {
        $cond = handlePreprocessorConditions($conds, $stmt);
        if ($stmt instanceof Stmt\Nop) {
            continue;
        }

        if ($stmt instanceof Stmt\Function_) {
            $funcInfos[] = parseFunctionLike($stmt->name->toString(), null, $stmt, $cond);
            continue;
        }

        if ($stmt instanceof Stmt\ClassLike) {
            $className = $stmt->name->toString();
            $methodInfos = [];
            foreach ($stmt->stmts as $classStmt) {
                $cond = handlePreprocessorConditions($conds, $classStmt);
                if ($classStmt instanceof Stmt\Nop) {
                    continue;
                }

                if (!$classStmt instanceof Stmt\ClassMethod) {
                    throw new Exception("Not implemented {$classStmt->getType()}");
                }

                $methodInfos[] = parseFunctionLike(
                    $classStmt->name->toString(), $className, $classStmt, $cond);
            }

            $classInfos[] = new ClassInfo($className, $methodInfos);
            continue;
        }

        throw new Exception("Unexpected node {$stmt->getType()}");
    }

    return new FileInfo($funcInfos, $classInfos, $generateFunctionEntries);
}

function funcInfoToCode(FuncInfo $funcInfo): string {
    $code = '';
    $returnType = $funcInfo->return->type;
    if ($returnType !== null) {
        if (null !== $simpleReturnType = $returnType->tryToSimpleType()) {
            if ($simpleReturnType->isBuiltin) {
                $code .= sprintf(
                    "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(%s, %d, %d, %s, %d)\n",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $simpleReturnType->toTypeCode(), $returnType->isNullable()
                );
            } else {
                $code .= sprintf(
                    "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(%s, %d, %d, %s, %d)\n",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $simpleReturnType->toEscapedName(), $returnType->isNullable()
                );
            }
        } else if (null !== $representableType = $returnType->tryToRepresentableType()) {
            if ($representableType->classType !== null) {
                $code .= sprintf(
                    "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(%s, %d, %d, %s, %s)\n",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $representableType->classType->toEscapedName(), $representableType->toTypeMask()
                );
            } else {
                $code .= sprintf(
                    "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(%s, %d, %d, %s)\n",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $representableType->toTypeMask()
                );
            }
        } else {
            throw new Exception('Unimplemented');
        }
    } else {
        $code .= sprintf(
            "ZEND_BEGIN_ARG_INFO_EX(%s, 0, %d, %d)\n",
            $funcInfo->getArgInfoName(), $funcInfo->return->byRef, $funcInfo->numRequiredArgs
        );
    }

    foreach ($funcInfo->args as $argInfo) {
        $argKind = $argInfo->isVariadic ? "ARG_VARIADIC" : "ARG";
        $argType = $argInfo->type;
        if ($argType !== null) {
            if (null !== $simpleArgType = $argType->tryToSimpleType()) {
                if ($simpleArgType->isBuiltin) {
                    $code .= sprintf(
                        "\tZEND_%s_TYPE_INFO(%s, %s, %s, %d)\n",
                        $argKind, $argInfo->getSendByString(), $argInfo->name,
                        $simpleArgType->toTypeCode(), $argType->isNullable()
                    );
                } else {
                    $code .= sprintf(
                        "\tZEND_%s_OBJ_INFO(%s, %s, %s, %d)\n",
                        $argKind, $argInfo->getSendByString(), $argInfo->name,
                        $simpleArgType->toEscapedName(), $argType->isNullable()
                    );
                }
            } else if (null !== $representableType = $argType->tryToRepresentableType()) {
                if ($representableType->classType !== null) {
                    throw new Exception('Unimplemented');
                }
                $code .= sprintf(
                    "\tZEND_%s_TYPE_MASK(%s, %s, %s)\n",
                    $argKind, $argInfo->getSendByString(), $argInfo->name,
                    $representableType->toTypeMask()
                );
            } else {
                throw new Exception('Unimplemented');
            }
        } else {
            $code .= sprintf(
                "\tZEND_%s_INFO(%s, %s)\n", $argKind, $argInfo->getSendByString(), $argInfo->name);
        }
    }

    $code .= "ZEND_END_ARG_INFO()";
    return $code . "\n";
}

function findEquivalentFuncInfo(array $generatedFuncInfos, $funcInfo): ?FuncInfo {
    foreach ($generatedFuncInfos as $generatedFuncInfo) {
        if ($generatedFuncInfo->equalsApartFromName($funcInfo)) {
            return $generatedFuncInfo;
        }
    }
    return null;
}

/** @param FuncInfo[] $funcInfos */
function generateCodeWithConditions(
        iterable $funcInfos, string $separator, Closure $codeGenerator): string {
    $code = "";
    foreach ($funcInfos as $funcInfo) {
        $funcCode = $codeGenerator($funcInfo);
        if ($funcCode === null) {
            continue;
        }

        $code .= $separator;
        if ($funcInfo->cond) {
            $code .= "#if {$funcInfo->cond}\n";
            $code .= $funcCode;
            $code .= "#endif\n";
        } else {
            $code .= $funcCode;
        }
    }
    return $code;
}

function generateArgInfoCode(FileInfo $fileInfo): string {
    $funcInfos = $fileInfo->funcInfos;

    $code = "/* This is a generated file, edit the .stub.php file instead. */\n";
    $generatedFuncInfos = [];
    $code .= generateCodeWithConditions(
        $fileInfo->getAllFuncInfos(), "\n",
        function(FuncInfo $funcInfo) use(&$generatedFuncInfos) {
            /* If there already is an equivalent arginfo structure, only emit a #define */
            if ($generatedFuncInfo = findEquivalentFuncInfo($generatedFuncInfos, $funcInfo)) {
                $code = sprintf(
                    "#define %s %s\n",
                    $funcInfo->getArgInfoName(), $generatedFuncInfo->getArgInfoName()
                );
            } else {
                $code = funcInfoToCode($funcInfo);
            }

            $generatedFuncInfos[] = $funcInfo;
            return $code;
        }
    );

    if ($fileInfo->generateFunctionEntries) {
        $code .= "\n\n";
        $code .= generateCodeWithConditions($fileInfo->funcInfos, "", function(FuncInfo $funcInfo) {
            if ($funcInfo->alias) {
                return null;
            }

            return "ZEND_FUNCTION($funcInfo->name);\n";
        });

        $code .= "\n\nstatic const zend_function_entry ext_functions[] = {\n";
        $code .= generateCodeWithConditions($fileInfo->funcInfos, "", function(FuncInfo $funcInfo) {
            if ($funcInfo->alias) {
                return sprintf(
                    "\tZEND_FALIAS(%s, %s, %s)\n",
                    $funcInfo->name, $funcInfo->alias, $funcInfo->getArgInfoName()
                );
            } else {
                return sprintf("\tZEND_FE(%s, %s)\n", $funcInfo->name, $funcInfo->getArgInfoName());
            }
        });
        $code .= "\tZEND_FE_END\n";
        $code .= "};\n";
    }

    return $code;
}

function initPhpParser() {
    $version = "4.3.0";
    $phpParserDir = __DIR__ . "/PHP-Parser-$version";
    if (!is_dir($phpParserDir)) {
        $cwd = getcwd();
        chdir(__DIR__);

        passthru("wget https://github.com/nikic/PHP-Parser/archive/v$version.tar.gz", $exit);
        if ($exit !== 0) {
            passthru("curl -LO https://github.com/nikic/PHP-Parser/archive/v$version.tar.gz", $exit);
        }
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
