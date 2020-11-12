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

/**
 * @return FileInfo[]
 */
function processDirectory(string $dir, Context $context): array {
    $fileInfos = [];

    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir),
        RecursiveIteratorIterator::LEAVES_ONLY
    );
    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (preg_match('/\.stub\.php$/', $pathName)) {
            $fileInfo = processStubFile($pathName, $context);
            if ($fileInfo) {
                $fileInfos[] = $fileInfo;
            }
        }
    }

    return $fileInfos;
}

function processStubFile(string $stubFile, Context $context): ?FileInfo {
    try {
        if (!file_exists($stubFile)) {
            throw new Exception("File $stubFile does not exist");
        }

        $arginfoFile = str_replace('.stub.php', '_arginfo.h', $stubFile);
        $legacyFile = str_replace('.stub.php', '_legacy_arginfo.h', $stubFile);

        $stubCode = file_get_contents($stubFile);
        $stubHash = computeStubHash($stubCode);
        $oldStubHash = extractStubHash($arginfoFile);
        if ($stubHash === $oldStubHash && !$context->forceParse) {
            /* Stub file did not change, do not regenerate. */
            return null;
        }

        initPhpParser();
        $fileInfo = parseStubFile($stubCode);
        $arginfoCode = generateArgInfoCode($fileInfo, $stubHash);
        if (($context->forceRegeneration || $stubHash !== $oldStubHash) && file_put_contents($arginfoFile, $arginfoCode)) {
            echo "Saved $arginfoFile\n";
        }

        if ($fileInfo->generateLegacyArginfo) {
            foreach ($fileInfo->getAllFuncInfos() as $funcInfo) {
                $funcInfo->discardInfoForOldPhpVersions();
            }
            $arginfoCode = generateArgInfoCode($fileInfo, $stubHash);
            if (($context->forceRegeneration || $stubHash !== $oldStubHash) && file_put_contents($legacyFile, $arginfoCode)) {
                echo "Saved $legacyFile\n";
            }
		}

        return $fileInfo;
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

class Context {
    /** @var bool */
    public $forceParse = false;
    /** @var bool */
    public $forceRegeneration = false;
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
            if ($node->toLowerString() === 'static') {
                // PHP internally considers "static" a builtin type.
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
        case "iterable":
            return "IS_ITERABLE";
        case "mixed":
            return "IS_MIXED";
        case "static":
            return "IS_STATIC";
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
        case "static":
            return "MAY_BE_STATIC";
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

interface FunctionOrMethodName {
    public function getDeclaration(): string;
    public function getArgInfoName(): string;
    public function __toString(): string;
    public function isMethod(): bool;
    public function isConstructor(): bool;
    public function isDestructor(): bool;
}

class FunctionName implements FunctionOrMethodName {
    /** @var Name */
    private $name;

    public function __construct(Name $name) {
        $this->name = $name;
    }

    public function getNamespace(): ?string {
        if ($this->name->isQualified()) {
            return $this->name->slice(0, -1)->toString();
        }
        return null;
    }

    public function getNonNamespacedName(): string {
        if ($this->name->isQualified()) {
            throw new Exception("Namespaced name not supported here");
        }
        return $this->name->toString();
    }

    public function getDeclarationName(): string {
        return $this->name->getLast();
    }

    public function getDeclaration(): string {
        return "ZEND_FUNCTION({$this->getDeclarationName()});\n";
    }

    public function getArgInfoName(): string {
        $underscoreName = implode('_', $this->name->parts);
        return "arginfo_$underscoreName";
    }

    public function __toString(): string {
        return $this->name->toString();
    }

    public function isMethod(): bool {
        return false;
    }

    public function isConstructor(): bool {
        return false;
    }

    public function isDestructor(): bool {
        return false;
    }
}

class MethodName implements FunctionOrMethodName {
    /** @var Name */
    private $className;
    /** @var string */
    public $methodName;

    public function __construct(Name $className, string $methodName) {
        $this->className = $className;
        $this->methodName = $methodName;
    }

    public function getDeclarationClassName(): string {
        return implode('_', $this->className->parts);
    }

    public function getDeclaration(): string {
        return "ZEND_METHOD({$this->getDeclarationClassName()}, $this->methodName);\n";
    }

    public function getArgInfoName(): string {
        return "arginfo_class_{$this->getDeclarationClassName()}_{$this->methodName}";
    }

    public function __toString(): string {
        return "$this->className::$this->methodName";
    }

    public function isMethod(): bool {
        return true;
    }

    public function isConstructor(): bool {
        return $this->methodName === "__construct";
    }

    public function isDestructor(): bool {
        return $this->methodName === "__destruct";
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
    /** @var FunctionOrMethodName */
    public $name;
    /** @var int */
    public $classFlags;
    /** @var int */
    public $flags;
    /** @var string|null */
    public $aliasType;
    /** @var FunctionName|null */
    public $alias;
    /** @var bool */
    public $isDeprecated;
    /** @var bool */
    public $verify;
    /** @var ArgInfo[] */
    public $args;
    /** @var ReturnInfo */
    public $return;
    /** @var int */
    public $numRequiredArgs;
    /** @var string|null */
    public $cond;

    public function __construct(
        FunctionOrMethodName $name,
        int $classFlags,
        int $flags,
        ?string $aliasType,
        ?FunctionOrMethodName $alias,
        bool $isDeprecated,
        bool $verify,
        array $args,
        ReturnInfo $return,
        int $numRequiredArgs,
        ?string $cond
    ) {
        $this->name = $name;
        $this->classFlags = $classFlags;
        $this->flags = $flags;
        $this->aliasType = $aliasType;
        $this->alias = $alias;
        $this->isDeprecated = $isDeprecated;
        $this->verify = $verify;
        $this->args = $args;
        $this->return = $return;
        $this->numRequiredArgs = $numRequiredArgs;
        $this->cond = $cond;
    }

    public function isMethod(): bool
    {
        return $this->name->isMethod();
    }

    public function isFinalMethod(): bool
    {
        return ($this->flags & Class_::MODIFIER_FINAL) || ($this->classFlags & Class_::MODIFIER_FINAL);
    }

    public function isInstanceMethod(): bool
    {
        return !($this->flags & Class_::MODIFIER_STATIC) && $this->isMethod() && !$this->name->isConstructor();
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
        return $this->name->getArgInfoName();
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
        if ($this->name instanceof MethodName) {
            if ($this->alias) {
                if ($this->alias instanceof MethodName) {
                    return sprintf(
                        "\tZEND_MALIAS(%s, %s, %s, %s, %s)\n",
                        $this->alias->getDeclarationClassName(), $this->name->methodName,
                        $this->alias->methodName, $this->getArgInfoName(), $this->getFlagsAsString()
                    );
                } else if ($this->alias instanceof FunctionName) {
                    return sprintf(
                        "\tZEND_ME_MAPPING(%s, %s, %s, %s)\n",
                        $this->name->methodName, $this->alias->getNonNamespacedName(),
                        $this->getArgInfoName(), $this->getFlagsAsString()
                    );
                } else {
                    throw new Error("Cannot happen");
                }
            } else {
                $declarationClassName = $this->name->getDeclarationClassName();
                if ($this->flags & Class_::MODIFIER_ABSTRACT) {
                    return sprintf(
                        "\tZEND_ABSTRACT_ME_WITH_FLAGS(%s, %s, %s, %s)\n",
                        $declarationClassName, $this->name->methodName, $this->getArgInfoName(),
                        $this->getFlagsAsString()
                    );
                }

                return sprintf(
                    "\tZEND_ME(%s, %s, %s, %s)\n",
                    $declarationClassName, $this->name->methodName, $this->getArgInfoName(),
                    $this->getFlagsAsString()
                );
            }
        } else if ($this->name instanceof FunctionName) {
            $namespace = $this->name->getNamespace();
            $declarationName = $this->name->getDeclarationName();

            if ($this->alias && $this->isDeprecated) {
                return sprintf(
                    "\tZEND_DEP_FALIAS(%s, %s, %s)\n",
                    $declarationName, $this->alias->getNonNamespacedName(), $this->getArgInfoName()
                );
            }

            if ($this->alias) {
                return sprintf(
                    "\tZEND_FALIAS(%s, %s, %s)\n",
                    $declarationName, $this->alias->getNonNamespacedName(), $this->getArgInfoName()
                );
            }

            if ($this->isDeprecated) {
                return sprintf(
                    "\tZEND_DEP_FE(%s, %s)\n", $declarationName, $this->getArgInfoName());
            }

            if ($namespace) {
                return sprintf(
                    "\tZEND_NS_FE(\"%s\", %s, %s)\n",
                    $namespace, $declarationName, $this->getArgInfoName());
            } else {
                return sprintf("\tZEND_FE(%s, %s)\n", $declarationName, $this->getArgInfoName());
            }
        } else {
            throw new Error("Cannot happen");
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

    public function discardInfoForOldPhpVersions(): void {
        $this->return->type = null;
        foreach ($this->args as $arg) {
            $arg->type = null;
            $arg->defaultValue = null;
        }
    }
}

class ClassInfo {
    /** @var Name */
    public $name;
    /** @var FuncInfo[] */
    public $funcInfos;

    public function __construct(Name $name, array $funcInfos) {
        $this->name = $name;
        $this->funcInfos = $funcInfos;
    }
}

class FileInfo {
    /** @var FuncInfo[] */
    public $funcInfos = [];
    /** @var ClassInfo[] */
    public $classInfos = [];
    /** @var bool */
    public $generateFunctionEntries = false;
    /** @var string */
    public $declarationPrefix = "";
    /** @var bool */
    public $generateLegacyArginfo = false;

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
        if ($value === null || strlen($value) === 0) {
            throw new Exception("@$this->name doesn't have any value");
        }

        $matches = [];

        if ($this->name === "param") {
            preg_match('/^\s*[\w\|\\\\]+\s*\$(\w+).*$/', $value, $matches);
        } elseif ($this->name === "prefer-ref") {
            preg_match('/^\s*\$(\w+).*$/', $value, $matches);
        }

        if (isset($matches[1]) === false) {
            throw new Exception("@$this->name doesn't contain variable name or has an invalid format \"$value\"");
        }

        return $matches[1];
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
    FunctionOrMethodName $name,
    int $classFlags,
    int $flags,
    Node\FunctionLike $func,
    ?string $cond
): FuncInfo {
    $comment = $func->getDocComment();
    $paramMeta = [];
    $aliasType = null;
    $alias = null;
    $isDeprecated = false;
    $verify = true;
    $haveDocReturnType = false;
    $docParamTypes = [];

    if ($comment) {
        $tags = parseDocComment($comment);
        foreach ($tags as $tag) {
            if ($tag->name === 'prefer-ref') {
                $varName = $tag->getVariableName();
                if (!isset($paramMeta[$varName])) {
                    $paramMeta[$varName] = [];
                }
                $paramMeta[$varName]['preferRef'] = true;
            } else if ($tag->name === 'alias' || $tag->name === 'implementation-alias') {
                $aliasType = $tag->name;
                $aliasParts = explode("::", $tag->getValue());
                if (count($aliasParts) === 1) {
                    $alias = new FunctionName(new Name($aliasParts[0]));
                } else {
                    $alias = new MethodName(new Name($aliasParts[0]), $aliasParts[1]);
                }
            } else if ($tag->name === 'deprecated') {
                $isDeprecated = true;
            }  else if ($tag->name === 'no-verify') {
                $verify = false;
            } else if ($tag->name === 'return') {
                $haveDocReturnType = true;
            } else if ($tag->name === 'param') {
                $docParamTypes[$tag->getVariableName()] = true;
            }
        }
    }

    $varNameSet = [];
    $args = [];
    $numRequiredArgs = 0;
    $foundVariadic = false;
    foreach ($func->getParams() as $i => $param) {
        $varName = $param->var->name;
        $preferRef = !empty($paramMeta[$varName]['preferRef']);
        unset($paramMeta[$varName]);

        if (isset($varNameSet[$varName])) {
            throw new Exception("Duplicate parameter name $varName for function $name");
        }
        $varNameSet[$varName] = true;

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
        if ($type === null && !isset($docParamTypes[$varName])) {
            throw new Exception("Missing parameter type for function $name()");
        }

        if ($param->default instanceof Expr\ConstFetch &&
            $param->default->name->toLowerString() === "null" &&
            $type && !$type->isNullable()
        ) {
            $simpleType = $type->tryToSimpleType();
            if ($simpleType === null) {
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
    if ($returnType === null && !$haveDocReturnType && !$name->isConstructor() && !$name->isDestructor()) {
        throw new Exception("Missing return type for function $name()");
    }

    $return = new ReturnInfo(
        $func->returnsByRef(),
        $returnType ? Type::fromNode($returnType) : null
    );

    return new FuncInfo(
        $name,
        $classFlags,
        $flags,
        $aliasType,
        $alias,
        $isDeprecated,
        $verify,
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

function handleStatements(FileInfo $fileInfo, array $stmts, PrettyPrinterAbstract $prettyPrinter) {
    $conds = [];
    foreach ($stmts as $stmt) {
        if ($stmt instanceof Stmt\Nop) {
            continue;
        }

        if ($stmt instanceof Stmt\Namespace_) {
            handleStatements($fileInfo, $stmt->stmts, $prettyPrinter);
            continue;
        }

        $cond = handlePreprocessorConditions($conds, $stmt);
        if ($stmt instanceof Stmt\Function_) {
            $fileInfo->funcInfos[] = parseFunctionLike(
                $prettyPrinter,
                new FunctionName($stmt->namespacedName),
                0,
                0,
                $stmt,
                $cond
            );
            continue;
        }

        if ($stmt instanceof Stmt\ClassLike) {
            $className = $stmt->namespacedName;
            $methodInfos = [];
            foreach ($stmt->stmts as $classStmt) {
                $cond = handlePreprocessorConditions($conds, $classStmt);
                if ($classStmt instanceof Stmt\Nop) {
                    continue;
                }

                if (!$classStmt instanceof Stmt\ClassMethod) {
                    throw new Exception("Not implemented {$classStmt->getType()}");
                }

                $classFlags = 0;
                if ($stmt instanceof Class_) {
                    $classFlags = $stmt->flags;
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
                    new MethodName($className, $classStmt->name->toString()),
                    $classFlags,
                    $flags,
                    $classStmt,
                    $cond
                );
            }

            $fileInfo->classInfos[] = new ClassInfo($className, $methodInfos);
            continue;
        }

        throw new Exception("Unexpected node {$stmt->getType()}");
    }
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

    $fileInfo = new FileInfo;
    $fileDocComment = getFileDocComment($stmts);
    if ($fileDocComment) {
        $fileTags = parseDocComment($fileDocComment);
        foreach ($fileTags as $tag) {
            if ($tag->name === 'generate-function-entries') {
                $fileInfo->generateFunctionEntries = true;
                $fileInfo->declarationPrefix = $tag->value ? $tag->value . " " : "";
            } else if ($tag->name === 'generate-legacy-arginfo') {
                $fileInfo->generateLegacyArginfo = true;
            }
        }
    }

    handleStatements($fileInfo, $stmts, $prettyPrinter);
    return $fileInfo;
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
            function (FuncInfo $funcInfo) use($fileInfo, &$generatedFunctionDeclarations) {
                $key = $funcInfo->getDeclarationKey();
                if (isset($generatedFunctionDeclarations[$key])) {
                    return null;
                }

                $generatedFunctionDeclarations[$key] = true;
                return $fileInfo->declarationPrefix . $funcInfo->getDeclaration();
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
function generateFunctionEntries(?Name $className, array $funcInfos): string {
    $code = "";

    $functionEntryName = "ext_functions";
    if ($className) {
        $underscoreName = implode("_", $className->parts);
        $functionEntryName = "class_{$underscoreName}_methods";
    }

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
    $version = "4.9.0";
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
$options = getopt("fh", ["force-regeneration", "parameter-stats", "help", "verify"], $optind);

$context = new Context;
$printParameterStats = isset($options["parameter-stats"]);
$verify = isset($options["verify"]);
$context->forceRegeneration = isset($options["f"]) || isset($options["force-regeneration"]);
$context->forceParse = $context->forceRegeneration || $printParameterStats || $verify;

if (isset($options["h"]) || isset($options["help"])) {
    die("\nusage: gen-stub.php [ -f | --force-regeneration ] [ --parameter-stats ] [ --verify ] [ -h | --help ] [ name.stub.php | directory ]\n\n");
}

$fileInfos = [];
$location = $argv[$optind] ?? ".";
if (is_file($location)) {
    // Generate single file.
    $fileInfo = processStubFile($location, $context);
    if ($fileInfo) {
        $fileInfos[] = $fileInfo;
    }
} else if (is_dir($location)) {
    $fileInfos = processDirectory($location, $context);
} else {
    echo "$location is neither a file nor a directory.\n";
    exit(1);
}

if ($printParameterStats) {
    $parameterStats = [];

    foreach ($fileInfos as $fileInfo) {
        foreach ($fileInfo->getAllFuncInfos() as $funcInfo) {
            foreach ($funcInfo->args as $argInfo) {
                if (!isset($parameterStats[$argInfo->name])) {
                    $parameterStats[$argInfo->name] = 0;
                }
                $parameterStats[$argInfo->name]++;
            }
        }
    }

    arsort($parameterStats);
    echo json_encode($parameterStats, JSON_PRETTY_PRINT), "\n";
}

if ($verify) {
    $errors = [];
    $funcMap = [];
    $aliases = [];

    foreach ($fileInfos as $fileInfo) {
        foreach ($fileInfo->getAllFuncInfos() as $funcInfo) {
            /** @var FuncInfo $funcInfo */
            $funcMap[$funcInfo->name->__toString()] = $funcInfo;

            if ($funcInfo->aliasType === "alias") {
                $aliases[] = $funcInfo;
            }
        }
    }

    foreach ($aliases as $aliasFunc) {
        if (!isset($funcMap[$aliasFunc->alias->__toString()])) {
            $errors[] = "Aliased function {$aliasFunc->alias}() cannot be found";
            continue;
        }

        if (!$aliasFunc->verify) {
            continue;
        }

        $aliasedFunc = $funcMap[$aliasFunc->alias->__toString()];
        $aliasedArgs = $aliasedFunc->args;
        $aliasArgs = $aliasFunc->args;

        if ($aliasFunc->isInstanceMethod() !== $aliasedFunc->isInstanceMethod()) {
            if ($aliasFunc->isInstanceMethod()) {
                $aliasedArgs = array_slice($aliasedArgs, 1);
            }

            if ($aliasedFunc->isInstanceMethod()) {
                $aliasArgs = array_slice($aliasArgs, 1);
            }
        }

        array_map(
            function(?ArgInfo $aliasArg, ?ArgInfo $aliasedArg) use ($aliasFunc, $aliasedFunc, &$errors) {
                if ($aliasArg === null) {
                    assert($aliasedArg !== null);
                    $errors[] = "{$aliasFunc->name}(): Argument \$$aliasedArg->name of aliased function {$aliasedFunc->name}() is missing";
                    return null;
                }

                if ($aliasedArg === null) {
                    assert($aliasArg !== null);
                    $errors[] = "{$aliasedFunc->name}(): Argument \$$aliasArg->name of alias function {$aliasFunc->name}() is missing";
                    return null;
                }

                if ($aliasArg->name !== $aliasedArg->name) {
                    $errors[] = "{$aliasFunc->name}(): Argument \$$aliasArg->name and argument \$$aliasedArg->name of aliased function {$aliasedFunc->name}() must have the same name";
                    return null;
                }

                if ($aliasArg->type != $aliasedArg->type) {
                    $errors[] = "{$aliasFunc->name}(): Argument \$$aliasArg->name and argument \$$aliasedArg->name of aliased function {$aliasedFunc->name}() must have the same type";
                }

                if ($aliasArg->defaultValue !== $aliasedArg->defaultValue) {
                    $errors[] = "{$aliasFunc->name}(): Argument \$$aliasArg->name and argument \$$aliasedArg->name of aliased function {$aliasedFunc->name}() must have the same default value";
                }
            },
            $aliasArgs, $aliasedArgs
        );

        if ((!$aliasedFunc->isMethod() || $aliasedFunc->isFinalMethod()) &&
            (!$aliasFunc->isMethod() || $aliasFunc->isFinalMethod()) &&
            $aliasFunc->return != $aliasedFunc->return
        ) {
            $errors[] = "{$aliasFunc->name}() and {$aliasedFunc->name}() must have the same return type";
        }
    }

    echo implode("\n", $errors);
    if (!empty($errors)) {
        echo "\n";
        exit(1);
    }
}
