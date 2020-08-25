#!/usr/bin/env php
<?php declare(strict_types=1);

use PhpParser\Comment\Doc as DocComment;
use PhpParser\Node;
use PhpParser\Node\Expr;
use PhpParser\Node\Name;
use PhpParser\Node\Stmt;
use PhpParser\Node\Stmt\Class_;
use PhpParser\PrettyPrinter\Standard;
use PhpParser\PrettyPrinterAbstract;

error_reporting(E_ALL);

function processDirectory(string $dir, bool $forceRegeneration) {
    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir),
        RecursiveIteratorIterator::LEAVES_ONLY
    );
    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (preg_match('/\.stub\.php$/', $pathName)) {
            processStubFile($pathName, $forceRegeneration);
        }
    }
}

function processStubFile(string $stubFile, bool $forceRegeneration) {
    try {
        if (!file_exists($stubFile)) {
            throw new Exception("File $stubFile does not exist");
        }

        $arginfoFile = str_replace('.stub.php', '', $stubFile) . '_arginfo.h';
        $stubCode = file_get_contents($stubFile);
        $stubHash = computeStubHash($stubCode);
        $oldStubHash = extractStubHash($arginfoFile);
        if ($stubHash === $oldStubHash && $forceRegeneration === false) {
            /* Stub file did not change, do not regenerate. */
            return;
        }

        initPhpParser();
        $fileInfo = parseStubFile($stubCode);
        $arginfoCode = generateArgInfoCode($fileInfo, $stubHash);
        file_put_contents($arginfoFile, $arginfoCode);
    } catch (Exception $e) {
        echo "In $stubFile:\n{$e->getMessage()}\n";
        exit(1);
    }
}

function computeStubHash(string $stubCode): string {
    return sha1(str_replace("\r\n", "\n", $stubCode));
}

function extractStubHash(string $arginfoFile): ?string {
    if (!file_exists($arginfoFile)) {
        return null;
    }

    $arginfoCode = file_get_contents($arginfoFile);
    if (!preg_match('/\* Stub hash: ([0-9a-f]+) \*/', $arginfoCode, $matches)) {
        return null;
    }

    return $matches[1];
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
            if ($node->toString() === "mixed") {
                return new SimpleType($node->toString(), true);
            }

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
        case "mixed":
            return "IS_MIXED";
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
        case "mixed":
            return "MAY_BE_ANY";
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
                return null;
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
    /** @var string|null */
    public $defaultValue;

    public function __construct(string $name, int $sendBy, bool $isVariadic, ?Type $type, ?string $defaultValue) {
        $this->name = $name;
        $this->sendBy = $sendBy;
        $this->isVariadic = $isVariadic;
        $this->type = $type;
        $this->defaultValue = $defaultValue;
    }

    public function equals(ArgInfo $other): bool {
        return $this->name === $other->name
            && $this->sendBy === $other->sendBy
            && $this->isVariadic === $other->isVariadic
            && Type::equals($this->type, $other->type)
            && $this->defaultValue === $other->defaultValue;
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

    public function hasDefaultValue(): bool {
        return $this->defaultValue !== null && $this->defaultValue !== "UNKNOWN";
    }

    public function getDefaultValueString(): string {
        if ($this->hasDefaultValue()) {
            return '"' . addslashes($this->defaultValue) . '"';
        }

        return "NULL";
    }
}

class FunctionName {
    /** @var string|null */
    public $className;
    /** @var string */
    public $name;

    public function __construct(?string $className, string $name)
    {
        $this->className = $className;
        $this->name = $name;
    }

    public function getDeclaration(): string
    {
        if ($this->className) {
            return "ZEND_METHOD($this->className, $this->name);\n";
        }

        return "ZEND_FUNCTION($this->name);\n";
    }

    public function __toString()
    {
        return $this->className ? "$this->className::$this->name" : $this->name;
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
    /** @var FunctionName */
    public $name;
    /** @var int */
    public $flags;
    /** @var FunctionName|null */
    public $alias;
    /** @var bool */
    public $isDeprecated;
    /** @var ArgInfo[] */
    public $args;
    /** @var ReturnInfo */
    public $return;
    /** @var int */
    public $numRequiredArgs;
    /** @var string|null */
    public $cond;

    public function __construct(
        FunctionName $name,
        int $flags,
        ?FunctionName $alias,
        bool $isDeprecated,
        array $args,
        ReturnInfo $return,
        int $numRequiredArgs,
        ?string $cond
    ) {
        $this->name = $name;
        $this->flags = $flags;
        $this->alias = $alias;
        $this->isDeprecated = $isDeprecated;
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
        if ($this->name->className) {
            return 'arginfo_class_' . $this->name->className . '_' . $this->name->name;
        }
        return 'arginfo_' . $this->name->name;
    }

    public function getDeclarationKey(): string
    {
        $name = $this->alias ?? $this->name;

        return "$name|$this->cond";
    }

    public function getDeclaration(): ?string
    {
        if ($this->flags & Class_::MODIFIER_ABSTRACT) {
            return null;
        }

        $name = $this->alias ?? $this->name;

        return $name->getDeclaration();
    }

    public function getFunctionEntry(): string {
        if ($this->name->className) {
            if ($this->alias) {
                if ($this->alias->className) {
                    return sprintf(
                        "\tZEND_MALIAS(%s, %s, %s, %s, %s)\n",
                        $this->alias->className, $this->name->name, $this->alias->name, $this->getArgInfoName(), $this->getFlagsAsString()
                    );
                } else {
                    return sprintf(
                        "\tZEND_ME_MAPPING(%s, %s, %s, %s)\n",
                        $this->name->name, $this->alias->name, $this->getArgInfoName(), $this->getFlagsAsString()
                    );
                }
            } else {
                if ($this->flags & Class_::MODIFIER_ABSTRACT) {
                    return sprintf(
                        "\tZEND_ABSTRACT_ME_WITH_FLAGS(%s, %s, %s, %s)\n",
                        $this->name->className, $this->name->name, $this->getArgInfoName(), $this->getFlagsAsString()
                    );
                }

                return sprintf(
                    "\tZEND_ME(%s, %s, %s, %s)\n",
                    $this->name->className, $this->name->name, $this->getArgInfoName(), $this->getFlagsAsString()
                );
            }
        } else {
            if ($this->alias && $this->isDeprecated) {
                return sprintf(
                    "\tZEND_DEP_FALIAS(%s, %s, %s)\n",
                    $this->name, $this->alias->name, $this->getArgInfoName()
                );
            }

            if ($this->alias) {
                return sprintf(
                    "\tZEND_FALIAS(%s, %s, %s)\n",
                    $this->name, $this->alias->name, $this->getArgInfoName()
                );
            }

            if ($this->isDeprecated) {
                return sprintf("\tZEND_DEP_FE(%s, %s)\n", $this->name, $this->getArgInfoName());
            }

            return sprintf("\tZEND_FE(%s, %s)\n", $this->name, $this->getArgInfoName());
        }
    }

    private function getFlagsAsString(): string
    {
        $flags = "ZEND_ACC_PUBLIC";
        if ($this->flags & Class_::MODIFIER_PROTECTED) {
            $flags = "ZEND_ACC_PROTECTED";
        } elseif ($this->flags & Class_::MODIFIER_PRIVATE) {
            $flags = "ZEND_ACC_PRIVATE";
        }

        if ($this->flags & Class_::MODIFIER_STATIC) {
            $flags .= "|ZEND_ACC_STATIC";
        }

        if ($this->flags & Class_::MODIFIER_FINAL) {
            $flags .= "|ZEND_ACC_FINAL";
        }

        if ($this->flags & Class_::MODIFIER_ABSTRACT) {
            $flags .= "|ZEND_ACC_ABSTRACT";
        }

        if ($this->isDeprecated) {
            $flags .= "|ZEND_ACC_DEPRECATED";
        }

        return $flags;
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

    /**
     * @return iterable<FuncInfo>
     */
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
    /** @var string|null */
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
        $regex = '/^\*\s*@([a-z-]+)(?:\s+(.+))?$/';
        if (preg_match($regex, trim($commentLine), $matches)) {
            $tags[] = new DocCommentTag($matches[1], $matches[2] ?? null);
        }
    }

    return $tags;
}

function parseFunctionLike(
    PrettyPrinterAbstract $prettyPrinter,
    FunctionName $name,
    int $flags,
    Node\FunctionLike $func,
    ?string $cond
): FuncInfo {
    $comment = $func->getDocComment();
    $paramMeta = [];
    $alias = null;
    $isDeprecated = false;
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
                $aliasParts = explode("::", $tag->getValue());
                if (count($aliasParts) === 1) {
                    $alias = new FunctionName(null, $aliasParts[0]);
                } else {
                    $alias = new FunctionName($aliasParts[0], $aliasParts[1]);
                }
            } else if ($tag->name === 'deprecated') {
                $isDeprecated = true;
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
            $simpleType = $type->tryToSimpleType();
            if ($simpleType === null || $simpleType->name !== "mixed") {
                throw new Exception(
                    "Parameter $varName of function $name has null default, but is not nullable");
            }
        }

        $foundVariadic = $param->variadic;

        $args[] = new ArgInfo(
            $varName,
            $sendBy,
            $param->variadic,
            $type,
            $param->default ? $prettyPrinter->prettyPrintExpr($param->default) : null
        );
        if (!$param->default && !$param->variadic) {
            $numRequiredArgs = $i + 1;
        }
    }

    foreach (array_keys($paramMeta) as $var) {
        throw new Exception("Found metadata for invalid param $var of function $name");
    }

    $returnType = $func->getReturnType();
    if ($returnType === null && !$haveDocReturnType && strpos($name->name, '__') !== 0) {
        throw new Exception("Missing return type for function $name()");
    }

    $return = new ReturnInfo(
        $func->returnsByRef(),
        $returnType ? Type::fromNode($returnType) : null
    );

    return new FuncInfo(
        $name,
        $flags,
        $alias,
        $isDeprecated,
        $args,
        $return,
        $numRequiredArgs,
        $cond
    );
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

function parseStubFile(string $code): FileInfo {
    $lexer = new PhpParser\Lexer();
    $parser = new PhpParser\Parser\Php7($lexer);
    $nodeTraverser = new PhpParser\NodeTraverser;
    $nodeTraverser->addVisitor(new PhpParser\NodeVisitor\NameResolver);
    $prettyPrinter = new class extends Standard {
        protected function pName_FullyQualified(Name\FullyQualified $node) {
            return implode('\\', $node->parts);
        }
    };

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
            $funcInfos[] = parseFunctionLike(
                $prettyPrinter,
                new FunctionName(null, $stmt->name->toString()),
                0,
                $stmt,
                $cond
            );

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

                $flags = $classStmt->flags;
                if ($stmt instanceof Stmt\Interface_) {
                    $flags |= Class_::MODIFIER_ABSTRACT;
                }

                if (!($flags & Class_::VISIBILITY_MODIFIER_MASK)) {
                    throw new Exception("Method visibility modifier is required");
                }

                $methodInfos[] = parseFunctionLike(
                    $prettyPrinter,
                    new FunctionName($className, $classStmt->name->toString()),
                    $flags,
                    $classStmt,
                    $cond
                );
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
        $argDefaultKind = $argInfo->hasDefaultValue() ? "_WITH_DEFAULT_VALUE" : "";
        $argType = $argInfo->type;
        if ($argType !== null) {
            if (null !== $simpleArgType = $argType->tryToSimpleType()) {
                if ($simpleArgType->isBuiltin) {
                    $code .= sprintf(
                        "\tZEND_%s_TYPE_INFO%s(%s, %s, %s, %d%s)\n",
                        $argKind, $argDefaultKind, $argInfo->getSendByString(), $argInfo->name,
                        $simpleArgType->toTypeCode(), $argType->isNullable(),
                        $argInfo->hasDefaultValue() ? ", " . $argInfo->getDefaultValueString() : ""
                    );
                } else {
                    $code .= sprintf(
                        "\tZEND_%s_OBJ_INFO%s(%s, %s, %s, %d%s)\n",
                        $argKind,$argDefaultKind, $argInfo->getSendByString(), $argInfo->name,
                        $simpleArgType->toEscapedName(), $argType->isNullable(),
                        $argInfo->hasDefaultValue() ? ", " . $argInfo->getDefaultValueString() : ""
                    );
                }
            } else if (null !== $representableType = $argType->tryToRepresentableType()) {
                if ($representableType->classType !== null) {
                    $code .= sprintf(
                        "\tZEND_%s_OBJ_TYPE_MASK(%s, %s, %s, %s, %s)\n",
                        $argKind, $argInfo->getSendByString(), $argInfo->name,
                        $representableType->classType->toEscapedName(),
                        $representableType->toTypeMask(),
                        $argInfo->getDefaultValueString()
                    );
                } else {
                    $code .= sprintf(
                        "\tZEND_%s_TYPE_MASK(%s, %s, %s, %s)\n",
                        $argKind, $argInfo->getSendByString(), $argInfo->name,
                        $representableType->toTypeMask(),
                        $argInfo->getDefaultValueString()
                    );
                }
            } else {
                throw new Exception('Unimplemented');
            }
        } else {
            $code .= sprintf(
                "\tZEND_%s_INFO%s(%s, %s%s)\n",
                $argKind, $argDefaultKind, $argInfo->getSendByString(), $argInfo->name,
                $argInfo->hasDefaultValue() ? ", " . $argInfo->getDefaultValueString() : ""
            );
        }
    }

    $code .= "ZEND_END_ARG_INFO()";
    return $code . "\n";
}

/** @param FuncInfo[] $generatedFuncInfos */
function findEquivalentFuncInfo(array $generatedFuncInfos, FuncInfo $funcInfo): ?FuncInfo {
    foreach ($generatedFuncInfos as $generatedFuncInfo) {
        if ($generatedFuncInfo->equalsApartFromName($funcInfo)) {
            return $generatedFuncInfo;
        }
    }
    return null;
}

/** @param iterable<FuncInfo> $funcInfos */
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

function generateArgInfoCode(FileInfo $fileInfo, string $stubHash): string {
    $code = "/* This is a generated file, edit the .stub.php file instead.\n"
          . " * Stub hash: $stubHash */\n";
    $generatedFuncInfos = [];
    $code .= generateCodeWithConditions(
        $fileInfo->getAllFuncInfos(), "\n",
        function (FuncInfo $funcInfo) use(&$generatedFuncInfos) {
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

        $generatedFunctionDeclarations = [];
        $code .= generateCodeWithConditions(
            $fileInfo->getAllFuncInfos(), "",
            function (FuncInfo $funcInfo) use(&$generatedFunctionDeclarations) {
                $key = $funcInfo->getDeclarationKey();
                if (isset($generatedFunctionDeclarations[$key])) {
                    return null;
                }

                $generatedFunctionDeclarations[$key] = true;

                return $funcInfo->getDeclaration();
            }
        );

        if (!empty($fileInfo->funcInfos)) {
            $code .= generateFunctionEntries(null, $fileInfo->funcInfos);
        }

        foreach ($fileInfo->classInfos as $classInfo) {
            $code .= generateFunctionEntries($classInfo->name, $classInfo->funcInfos);
        }
    }

    return $code;
}

/** @param FuncInfo[] $funcInfos */
function generateFunctionEntries(?string $className, array $funcInfos): string {
    $code = "";

    $functionEntryName = $className ? "class_{$className}_methods" : "ext_functions";

    $code .= "\n\nstatic const zend_function_entry {$functionEntryName}[] = {\n";
    $code .= generateCodeWithConditions($funcInfos, "", function (FuncInfo $funcInfo) {
        return $funcInfo->getFunctionEntry();
    });
    $code .= "\tZEND_FE_END\n";
    $code .= "};\n";

    return $code;
}

function installPhpParser(string $version, string $phpParserDir) {
    $lockFile = __DIR__ . "/PHP-Parser-install-lock";
    $lockFd = fopen($lockFile, 'w+');
    if (!flock($lockFd, LOCK_EX)) {
        throw new Exception("Failed to acquire installation lock");
    }

    try {
        // Check whether a parallel process has already installed PHP-Parser.
        if (is_dir($phpParserDir)) {
            return;
        }

        $cwd = getcwd();
        chdir(__DIR__);

        $tarName = "v$version.tar.gz";
        passthru("wget https://github.com/nikic/PHP-Parser/archive/$tarName", $exit);
        if ($exit !== 0) {
            passthru("curl -LO https://github.com/nikic/PHP-Parser/archive/$tarName", $exit);
        }
        if ($exit !== 0) {
            throw new Exception("Failed to download PHP-Parser tarball");
        }
        if (!mkdir($phpParserDir)) {
            throw new Exception("Failed to create directory $phpParserDir");
        }
        passthru("tar xvzf $tarName -C PHP-Parser-$version --strip-components 1", $exit);
        if ($exit !== 0) {
            throw new Exception("Failed to extract PHP-Parser tarball");
        }
        unlink(__DIR__ . "/$tarName");
        chdir($cwd);
    } finally {
        flock($lockFd, LOCK_UN);
        @unlink($lockFile);
    }
}

function initPhpParser() {
    static $isInitialized = false;
    if ($isInitialized) {
        return;
    }

    if (!extension_loaded("tokenizer")) {
        throw new Exception("The \"tokenizer\" extension is not available");
    }

    $isInitialized = true;
    $version = "4.3.0";
    $phpParserDir = __DIR__ . "/PHP-Parser-$version";
    if (!is_dir($phpParserDir)) {
        installPhpParser($version, $phpParserDir);
    }

    spl_autoload_register(function(string $class) use($phpParserDir) {
        if (strpos($class, "PhpParser\\") === 0) {
            $fileName = $phpParserDir . "/lib/" . str_replace("\\", "/", $class) . ".php";
            require $fileName;
        }
    });
}

$optind = null;
$options = getopt("f", ["force-regeneration"], $optind);
$forceRegeneration = isset($options["f"]) || isset($options["force-regeneration"]);
$location = $argv[$optind + 1] ?? ".";

if (is_file($location)) {
    // Generate single file.
    processStubFile($location, $forceRegeneration);
} else if (is_dir($location)) {
    processDirectory($location, $forceRegeneration);
} else {
    echo "$location is neither a file nor a directory.\n";
    exit(1);
}
