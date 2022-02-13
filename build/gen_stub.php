#!/usr/bin/env php
<?php declare(strict_types=1);

use PhpParser\Comment\Doc as DocComment;
use PhpParser\ConstExprEvaluator;
use PhpParser\Node;
use PhpParser\Node\Expr;
use PhpParser\Node\Name;
use PhpParser\Node\Stmt;
use PhpParser\Node\Stmt\Class_;
use PhpParser\Node\Stmt\Enum_;
use PhpParser\Node\Stmt\Interface_;
use PhpParser\Node\Stmt\Trait_;
use PhpParser\PrettyPrinter\Standard;
use PhpParser\PrettyPrinterAbstract;

error_reporting(E_ALL);

/**
 * @return FileInfo[]
 */
function processDirectory(string $dir, Context $context): array {
    $pathNames = [];
    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir),
        RecursiveIteratorIterator::LEAVES_ONLY
    );
    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (preg_match('/\.stub\.php$/', $pathName)) {
            $pathNames[] = $pathName;
        }
    }

    // Make sure stub files are processed in a predictable, system-independent order.
    sort($pathNames);

    $fileInfos = [];
    foreach ($pathNames as $pathName) {
        $fileInfo = processStubFile($pathName, $context);
        if ($fileInfo) {
            $fileInfos[] = $fileInfo;
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
            $legacyFileInfo = clone $fileInfo;

            foreach ($legacyFileInfo->getAllFuncInfos() as $funcInfo) {
                $funcInfo->discardInfoForOldPhpVersions();
            }
            foreach ($legacyFileInfo->getAllPropertyInfos() as $propertyInfo) {
                $propertyInfo->discardInfoForOldPhpVersions();
            }

            $arginfoCode = generateArgInfoCode($legacyFileInfo, $stubHash);
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

class ArrayType extends SimpleType {
    /** @var Type */
    public $keyType;

    /** @var Type */
    public $valueType;

    public static function createGenericArray(): self
    {
        return new ArrayType(Type::fromString("int|string"), Type::fromString("mixed|ref"));
    }

    public function __construct(Type $keyType, Type $valueType)
    {
        parent::__construct("array", true);

        $this->keyType = $keyType;
        $this->valueType = $valueType;
    }

    public function toOptimizerTypeMask(): string {
        $typeMasks = [
            parent::toOptimizerTypeMask(),
            $this->keyType->toOptimizerTypeMaskForArrayKey(),
            $this->valueType->toOptimizerTypeMaskForArrayValue(),
        ];

        return implode("|", $typeMasks);
    }

    public function equals(SimpleType $other): bool {
        if (!parent::equals($other)) {
            return false;
        }

        assert(get_class($other) === self::class);

        return Type::equals($this->keyType, $other->keyType) &&
            Type::equals($this->valueType, $other->valueType);
    }
}

class SimpleType {
    /** @var string */
    public $name;
    /** @var bool */
    public $isBuiltin;

    public static function fromNode(Node $node): SimpleType {
        if ($node instanceof Node\Name) {
            if ($node->toLowerString() === 'static') {
                // PHP internally considers "static" a builtin type.
                return new SimpleType($node->toLowerString(), true);
            }

            if ($node->toLowerString() === 'self') {
                throw new Exception('The exact class name must be used instead of "self"');
            }

            assert($node->isFullyQualified());
            return new SimpleType($node->toString(), false);
        }

        if ($node instanceof Node\Identifier) {
            if ($node->toLowerString() === 'array') {
                return ArrayType::createGenericArray();
            }

            return new SimpleType($node->toLowerString(), true);
        }

        throw new Exception("Unexpected node type");
    }

    public static function fromString(string $typeString): SimpleType
    {
        switch (strtolower($typeString)) {
            case "void":
            case "null":
            case "false":
            case "true":
            case "bool":
            case "int":
            case "float":
            case "string":
            case "callable":
            case "iterable":
            case "object":
            case "resource":
            case "mixed":
            case "static":
            case "never":
            case "ref":
                return new SimpleType(strtolower($typeString), true);
            case "array":
                return ArrayType::createGenericArray();
            case "self":
                throw new Exception('The exact class name must be used instead of "self"');
        }

        $matches = [];
        $isArray = preg_match("/(.*)\s*\[\s*\]/", $typeString, $matches);
        if ($isArray) {
            return new ArrayType(Type::fromString("int"), Type::fromString($matches[1]));
        }

        $matches = [];
        $isArray = preg_match("/array\s*<\s*([A-Za-z0-9_-|]+)?(\s*,\s*)?([A-Za-z0-9_-|]+)?\s*>/i", $typeString, $matches);
        if ($isArray) {
            if (empty($matches[1]) || empty($matches[3])) {
                throw new Exception("array<> type hint must have both a key and a value");
            }

            return new ArrayType(Type::fromString($matches[1]), Type::fromString($matches[3]));
        }

        return new SimpleType($typeString, false);
    }

    public static function null(): SimpleType
    {
        return new SimpleType("null", true);
    }

    public static function void(): SimpleType
    {
        return new SimpleType("void", true);
    }

    protected function __construct(string $name, bool $isBuiltin) {
        $this->name = $name;
        $this->isBuiltin = $isBuiltin;
    }

    public function isScalar(): bool {
        return $this->isBuiltin && in_array($this->name, ["null", "false", "true", "bool", "int", "float"], true);
    }

    public function isNull(): bool {
        return $this->isBuiltin && $this->name === 'null';
    }

    public function toTypeCode(): string {
        assert($this->isBuiltin);
        switch ($this->name) {
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
            case "never":
                return "IS_NEVER";
            default:
                throw new Exception("Not implemented: $this->name");
        }
    }

    public function toTypeMask(): string {
        assert($this->isBuiltin);

        switch ($this->name) {
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
            case "iterable":
                return "MAY_BE_ITERABLE";
            case "mixed":
                return "MAY_BE_ANY";
            case "void":
                return "MAY_BE_VOID";
            case "static":
                return "MAY_BE_STATIC";
            case "never":
                return "MAY_BE_NEVER";
            default:
                throw new Exception("Not implemented: $this->name");
        }
    }

    public function toOptimizerTypeMaskForArrayKey(): string {
        assert($this->isBuiltin);

        switch ($this->name) {
            case "int":
                return "MAY_BE_ARRAY_KEY_LONG";
            case "string":
                return "MAY_BE_ARRAY_KEY_STRING";
            default:
                throw new Exception("Type $this->name cannot be an array key");
        }
    }

    public function toOptimizerTypeMaskForArrayValue(): string {
        if (!$this->isBuiltin) {
            return "MAY_BE_ARRAY_OF_OBJECT";
        }

        switch ($this->name) {
            case "null":
                return "MAY_BE_ARRAY_OF_NULL";
            case "false":
                return "MAY_BE_ARRAY_OF_FALSE";
            case "bool":
                return "MAY_BE_ARRAY_OF_FALSE|MAY_BE_ARRAY_OF_TRUE";
            case "int":
                return "MAY_BE_ARRAY_OF_LONG";
            case "float":
                return "MAY_BE_ARRAY_OF_DOUBLE";
            case "string":
                return "MAY_BE_ARRAY_OF_STRING";
            case "array":
                return "MAY_BE_ARRAY_OF_ARRAY";
            case "object":
                return "MAY_BE_ARRAY_OF_OBJECT";
            case "resource":
                return "MAY_BE_ARRAY_OF_RESOURCE";
            case "mixed":
                return "MAY_BE_ARRAY_OF_ANY";
            case "ref":
                return "MAY_BE_ARRAY_OF_REF";
            default:
                throw new Exception("Type $this->name cannot be an array value");
        }
    }

    public function toOptimizerTypeMask(): string {
        if (!$this->isBuiltin) {
            return "MAY_BE_OBJECT";
        }

        switch ($this->name) {
            case "true":
                return "MAY_BE_TRUE";
            case "resource":
                return "MAY_BE_RESOURCE";
            case "callable":
                return "MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_OBJECT";
            case "iterable":
                return "MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_ANY|MAY_BE_ARRAY_OF_ANY|MAY_BE_OBJECT";
            case "mixed":
                return "MAY_BE_ANY|MAY_BE_ARRAY_KEY_ANY|MAY_BE_ARRAY_OF_ANY";
        }

        return $this->toTypeMask();
    }

    public function toEscapedName(): string {
        return str_replace('\\', '\\\\', $this->name);
    }

    public function toVarEscapedName(): string {
        $name = str_replace('_', '__', $this->name);
        return str_replace('\\', '_', $this->name);
    }

    public function equals(SimpleType $other): bool {
        return $this->name === $other->name && $this->isBuiltin === $other->isBuiltin;
    }
}

class Type {
    /** @var SimpleType[] */
    public $types;

    public static function fromNode(Node $node): Type {
        if ($node instanceof Node\UnionType) {
            return new Type(array_map(['SimpleType', 'fromNode'], $node->types));
        }

        if ($node instanceof Node\NullableType) {
            return new Type(
                [
                    SimpleType::fromNode($node->type),
                    SimpleType::null(),
                ]
            );
        }

        return new Type([SimpleType::fromNode($node)]);
    }

    public static function fromString(string $typeString): self {
        $typeString .= "|";
        $simpleTypes = [];
        $simpleTypeOffset = 0;
        $inArray = false;

        $typeStringLength = strlen($typeString);
        for ($i = 0; $i < $typeStringLength; $i++) {
            $char = $typeString[$i];

            if ($char === "<") {
                $inArray = true;
                continue;
            }

            if ($char === ">") {
                $inArray = false;
                continue;
            }

            if ($inArray) {
                continue;
            }

            if ($char === "|") {
                $simpleTypeName = trim(substr($typeString, $simpleTypeOffset, $i - $simpleTypeOffset));

                $simpleTypes[] = SimpleType::fromString($simpleTypeName);

                $simpleTypeOffset = $i + 1;
            }
        }

        return new Type($simpleTypes);
    }

    /**
     * @param SimpleType[] $types
     */
    private function __construct(array $types) {
        $this->types = $types;
    }

    public function isScalar(): bool {
        foreach ($this->types as $type) {
            if (!$type->isScalar()) {
                return false;
            }
        }

        return true;
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
        return new Type(
            array_filter(
                $this->types,
                function(SimpleType $type) {
                    return !$type->isNull();
                }
            )
        );
    }

    public function tryToSimpleType(): ?SimpleType {
        $withoutNull = $this->getWithoutNull();
        if (count($withoutNull->types) === 1) {
            return $withoutNull->types[0];
        }
        return null;
    }

    public function toArginfoType(): ArginfoType {
        $classTypes = [];
        $builtinTypes = [];
        foreach ($this->types as $type) {
            if ($type->isBuiltin) {
                $builtinTypes[] = $type;
            } else {
                $classTypes[] = $type;
            }
        }
        return new ArginfoType($classTypes, $builtinTypes);
    }

    public function toOptimizerTypeMask(): string {
        $optimizerTypes = [];

        foreach ($this->types as $type) {
            $optimizerTypes[] = $type->toOptimizerTypeMask();
        }

        return implode("|", $optimizerTypes);
    }

    public function toOptimizerTypeMaskForArrayKey(): string {
        $typeMasks = [];

        foreach ($this->types as $type) {
            $typeMasks[] = $type->toOptimizerTypeMaskForArrayKey();
        }

        return implode("|", $typeMasks);
    }

    public function toOptimizerTypeMaskForArrayValue(): string {
        $typeMasks = [];

        foreach ($this->types as $type) {
            $typeMasks[] = $type->toOptimizerTypeMaskForArrayValue();
        }

        return implode("|", $typeMasks);
    }

    public function getTypeForDoc(DOMDocument $doc): DOMElement {
        if (count($this->types) > 1) {
            $typeElement = $doc->createElement('type');
            $typeElement->setAttribute("class", "union");

            foreach ($this->types as $type) {
                $unionTypeElement = $doc->createElement('type', $type->name);
                $typeElement->appendChild($unionTypeElement);
            }
        } else {
            $type = $this->types[0];
            if ($type->isBuiltin && strtolower($type->name) === "true") {
                $name = "bool";
            } else {
                $name = $type->name;
            }

            $typeElement = $doc->createElement('type', $name);
        }

        return $typeElement;
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

    public function __toString() {
        if ($this->types === null) {
            return 'mixed';
        }

        return implode('|', array_map(
            function ($type) { return $type->name; },
            $this->types)
        );
    }
}

class ArginfoType {
    /** @var SimpleType[] $classTypes */
    public $classTypes;

    /** @var SimpleType[] $builtinTypes */
    private $builtinTypes;

    /**
     * @param SimpleType[] $classTypes
     * @param SimpleType[] $builtinTypes
     */
    public function __construct(array $classTypes, array $builtinTypes) {
        $this->classTypes = $classTypes;
        $this->builtinTypes = $builtinTypes;
    }

    public function hasClassType(): bool {
        return !empty($this->classTypes);
    }

    public function toClassTypeString(): string {
        return implode('|', array_map(function(SimpleType $type) {
            return $type->toEscapedName();
        }, $this->classTypes));
    }

    public function toTypeMask(): string {
        if (empty($this->builtinTypes)) {
            return '0';
        }
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
    /** @var Type|null */
    public $phpDocType;
    /** @var string|null */
    public $defaultValue;

    public function __construct(string $name, int $sendBy, bool $isVariadic, ?Type $type, ?Type $phpDocType, ?string $defaultValue) {
        $this->name = $name;
        $this->sendBy = $sendBy;
        $this->isVariadic = $isVariadic;
        $this->setTypes($type, $phpDocType);
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

    public function getMethodSynopsisType(): Type {
        if ($this->type) {
            return $this->type;
        }

        if ($this->phpDocType) {
            return $this->phpDocType;
        }

        throw new Exception("A parameter must have a type");
    }

    public function hasProperDefaultValue(): bool {
        return $this->defaultValue !== null && $this->defaultValue !== "UNKNOWN";
    }

    public function getDefaultValueAsArginfoString(): string {
        if ($this->hasProperDefaultValue()) {
            return '"' . addslashes($this->defaultValue) . '"';
        }

        return "NULL";
    }

    public function getDefaultValueAsMethodSynopsisString(): ?string {
        if ($this->defaultValue === null) {
            return null;
        }

        switch ($this->defaultValue) {
            case 'UNKNOWN':
                return null;
            case 'false':
            case 'true':
            case 'null':
                return "&{$this->defaultValue};";
        }

        return $this->defaultValue;
    }

    private function setTypes(?Type $type, ?Type $phpDocType): void
    {
        if ($phpDocType !== null && Type::equals($type, $phpDocType)) {
            throw new Exception('PHPDoc param type "' . $phpDocType->__toString() . '" is unnecessary');
        }

        $this->type = $type;
        $this->phpDocType = $phpDocType;
    }
}

class PropertyName {
    /** @var Name */
    public $class;
    /** @var string */
    public $property;

    public function __construct(Name $class, string $property)
    {
        $this->class = $class;
        $this->property = $property;
    }

    public function __toString()
    {
        return $this->class->toString() . "::$" . $this->property;
    }
}

interface FunctionOrMethodName {
    public function getDeclaration(): string;
    public function getArgInfoName(): string;
    public function getMethodSynopsisFilename(): string;
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

    public function getMethodSynopsisFilename(): string {
        return implode('_', $this->name->parts);
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

    public function getMethodSynopsisFilename(): string {
        return $this->getDeclarationClassName() . "_{$this->methodName}";
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
    const REFCOUNT_0 = "0";
    const REFCOUNT_1 = "1";
    const REFCOUNT_N = "N";

    const REFCOUNTS = [
        self::REFCOUNT_0,
        self::REFCOUNT_1,
        self::REFCOUNT_N,
    ];

    /** @var bool */
    public $byRef;
    /** @var Type|null */
    public $type;
    /** @var Type|null */
    public $phpDocType;
    /** @var bool */
    public $tentativeReturnType;
    /** @var string */
    public $refcount;

    public function __construct(bool $byRef, ?Type $type, ?Type $phpDocType, bool $tentativeReturnType, ?string $refcount) {
        $this->byRef = $byRef;
        $this->setTypes($type, $phpDocType, $tentativeReturnType);
        $this->setRefcount($refcount);
    }

    public function equalsApartFromPhpDocAndRefcount(ReturnInfo $other): bool {
        return $this->byRef === $other->byRef
            && Type::equals($this->type, $other->type)
            && $this->tentativeReturnType === $other->tentativeReturnType;
    }

    public function getMethodSynopsisType(): ?Type {
        return $this->type ?? $this->phpDocType;
    }

    private function setTypes(?Type $type, ?Type $phpDocType, bool $tentativeReturnType): void
    {
        if ($phpDocType !== null && Type::equals($type, $phpDocType)) {
            throw new Exception('PHPDoc return type "' . $phpDocType->__toString() . '" is unnecessary');
        }

        $this->type = $type;
        $this->phpDocType = $phpDocType;
        $this->tentativeReturnType = $tentativeReturnType;
    }

    private function setRefcount(?string $refcount): void
    {
        $type = $this->phpDocType ?? $this->type;
        $isScalarType = $type !== null && $type->isScalar();

        if ($refcount === null) {
            $this->refcount = $isScalarType ? self::REFCOUNT_0 : self::REFCOUNT_N;
            return;
        }

        if (!in_array($refcount, ReturnInfo::REFCOUNTS, true)) {
            throw new Exception("@refcount must have one of the following values: \"0\", \"1\", \"N\", $refcount given");
        }

        if ($isScalarType && $refcount !== self::REFCOUNT_0) {
            throw new Exception('A scalar return type of "' . $type->__toString() . '" must have a refcount of "' . self::REFCOUNT_0 . '"');
        }

        if (!$isScalarType && $refcount === self::REFCOUNT_0) {
            throw new Exception('A non-scalar return type of "' . $type->__toString() . '" cannot have a refcount of "' . self::REFCOUNT_0 . '"');
        }

        $this->refcount = $refcount;
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

    /**
     * @param ArgInfo[] $args
     */
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

    /** @return string[] */
    public function getModifierNames(): array
    {
        if (!$this->isMethod()) {
            return [];
        }

        $result = [];

        if ($this->flags & Class_::MODIFIER_FINAL) {
            $result[] = "final";
        } elseif ($this->flags & Class_::MODIFIER_ABSTRACT && $this->classFlags & ~Class_::MODIFIER_ABSTRACT) {
            $result[] = "abstract";
        }

        if ($this->flags & Class_::MODIFIER_PROTECTED) {
            $result[] = "protected";
        } elseif ($this->flags & Class_::MODIFIER_PRIVATE) {
            $result[] = "private";
        } else {
            $result[] = "public";
        }

        if ($this->flags & Class_::MODIFIER_STATIC) {
            $result[] = "static";
        }

        return $result;
    }

    public function hasParamWithUnknownDefaultValue(): bool
    {
        foreach ($this->args as $arg) {
            if ($arg->defaultValue && !$arg->hasProperDefaultValue()) {
                return true;
            }
        }

        return false;
    }

    public function equalsApartFromNameAndRefcount(FuncInfo $other): bool {
        if (count($this->args) !== count($other->args)) {
            return false;
        }

        for ($i = 0; $i < count($this->args); $i++) {
            if (!$this->args[$i]->equals($other->args[$i])) {
                return false;
            }
        }

        return $this->return->equalsApartFromPhpDocAndRefcount($other->return)
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
                        $this->alias->methodName, $this->getArgInfoName(), $this->getFlagsAsArginfoString()
                    );
                } else if ($this->alias instanceof FunctionName) {
                    return sprintf(
                        "\tZEND_ME_MAPPING(%s, %s, %s, %s)\n",
                        $this->name->methodName, $this->alias->getNonNamespacedName(),
                        $this->getArgInfoName(), $this->getFlagsAsArginfoString()
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
                        $this->getFlagsAsArginfoString()
                    );
                }

                return sprintf(
                    "\tZEND_ME(%s, %s, %s, %s)\n",
                    $declarationClassName, $this->name->methodName, $this->getArgInfoName(),
                    $this->getFlagsAsArginfoString()
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
                // Render A\B as "A\\B" in C strings for namespaces
                return sprintf(
                    "\tZEND_NS_FE(\"%s\", %s, %s)\n",
                    addslashes($namespace), $declarationName, $this->getArgInfoName());
            } else {
                return sprintf("\tZEND_FE(%s, %s)\n", $declarationName, $this->getArgInfoName());
            }
        } else {
            throw new Error("Cannot happen");
        }
    }

    public function getOptimizerInfo(): ?string {
        if ($this->isMethod()) {
            return null;
        }

        if ($this->alias !== null) {
            return null;
        }

        if ($this->return->refcount !== ReturnInfo::REFCOUNT_1 && $this->return->phpDocType === null) {
            return null;
        }

        $type = $this->return->phpDocType ?? $this->return->type;
        if ($type === null) {
            return null;
        }

        return "    F" . $this->return->refcount . '("' . $this->name->__toString() . '", ' . $type->toOptimizerTypeMask() . "),\n";
    }

    public function discardInfoForOldPhpVersions(): void {
        $this->return->type = null;
        foreach ($this->args as $arg) {
            $arg->type = null;
            $arg->defaultValue = null;
        }
    }

    private function getFlagsAsArginfoString(): string
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

    /**
     * @param array<string, FuncInfo> $funcMap
     * @param array<string, FuncInfo> $aliasMap
     * @throws Exception
     */
    public function getMethodSynopsisDocument(array $funcMap, array $aliasMap): ?string {

        $doc = new DOMDocument();
        $doc->formatOutput = true;
        $methodSynopsis = $this->getMethodSynopsisElement($funcMap, $aliasMap, $doc);
        if (!$methodSynopsis) {
            return null;
        }

        $doc->appendChild($methodSynopsis);

        return $doc->saveXML();
    }

    /**
     * @param array<string, FuncInfo> $funcMap
     * @param array<string, FuncInfo> $aliasMap
     * @throws Exception
     */
    public function getMethodSynopsisElement(array $funcMap, array $aliasMap, DOMDocument $doc): ?DOMElement {
        if ($this->hasParamWithUnknownDefaultValue()) {
            return null;
        }

        if ($this->name->isConstructor()) {
            $synopsisType = "constructorsynopsis";
        } elseif ($this->name->isDestructor()) {
            $synopsisType = "destructorsynopsis";
        } else {
            $synopsisType = "methodsynopsis";
        }

        $methodSynopsis = $doc->createElement($synopsisType);

        $aliasedFunc = $this->aliasType === "alias" && isset($funcMap[$this->alias->__toString()]) ? $funcMap[$this->alias->__toString()] : null;
        $aliasFunc = $aliasMap[$this->name->__toString()] ?? null;

        if (($this->aliasType === "alias" && $aliasedFunc !== null && $aliasedFunc->isMethod() !== $this->isMethod()) ||
            ($aliasFunc !== null && $aliasFunc->isMethod() !== $this->isMethod())
        ) {
            $role = $doc->createAttribute("role");
            $role->value = $this->isMethod() ? "oop" : "procedural";
            $methodSynopsis->appendChild($role);
        }

        $methodSynopsis->appendChild(new DOMText("\n   "));

        foreach ($this->getModifierNames() as $modifierString) {
            $modifierElement = $doc->createElement('modifier', $modifierString);
            $methodSynopsis->appendChild($modifierElement);
            $methodSynopsis->appendChild(new DOMText(" "));
        }

        $returnType = $this->return->getMethodSynopsisType();
        if ($returnType) {
            $methodSynopsis->appendChild($returnType->getTypeForDoc($doc));
        }

        $methodname = $doc->createElement('methodname', $this->name->__toString());
        $methodSynopsis->appendChild($methodname);

        if (empty($this->args)) {
            $methodSynopsis->appendChild(new DOMText("\n   "));
            $void = $doc->createElement('void');
            $methodSynopsis->appendChild($void);
        } else {
            foreach ($this->args as $arg) {
                $methodSynopsis->appendChild(new DOMText("\n   "));
                $methodparam = $doc->createElement('methodparam');
                if ($arg->defaultValue !== null) {
                    $methodparam->setAttribute("choice", "opt");
                }
                if ($arg->isVariadic) {
                    $methodparam->setAttribute("rep", "repeat");
                }

                $methodSynopsis->appendChild($methodparam);
                $methodparam->appendChild($arg->getMethodSynopsisType()->getTypeForDoc($doc));

                $parameter = $doc->createElement('parameter', $arg->name);
                if ($arg->sendBy !== ArgInfo::SEND_BY_VAL) {
                    $parameter->setAttribute("role", "reference");
                }

                $methodparam->appendChild($parameter);
                $defaultValue = $arg->getDefaultValueAsMethodSynopsisString();
                if ($defaultValue !== null) {
                    $initializer = $doc->createElement('initializer');
                    if (preg_match('/^[a-zA-Z_][a-zA-Z_0-9]*$/', $defaultValue)) {
                        $constant = $doc->createElement('constant', $defaultValue);
                        $initializer->appendChild($constant);
                    } else {
                        $initializer->nodeValue = $defaultValue;
                    }
                    $methodparam->appendChild($initializer);
                }
            }
        }
        $methodSynopsis->appendChild(new DOMText("\n  "));

        return $methodSynopsis;
    }

    public function __clone()
    {
        foreach ($this->args as $key => $argInfo) {
            $this->args[$key] = clone $argInfo;
        }
        $this->return = clone $this->return;
    }
}

function initializeZval(string $zvalName, $value): string
{
    $code = "\tzval $zvalName;\n";

    switch (gettype($value)) {
        case "NULL":
            $code .= "\tZVAL_NULL(&$zvalName);\n";
            break;

        case "boolean":
            $code .= "\tZVAL_BOOL(&$zvalName, " . ((int) $value) . ");\n";
            break;

        case "integer":
            $code .= "\tZVAL_LONG(&$zvalName, $value);\n";
            break;

        case "double":
            $code .= "\tZVAL_DOUBLE(&$zvalName, $value);\n";
            break;

        case "string":
            if ($value === "") {
                $code .= "\tZVAL_EMPTY_STRING(&$zvalName);\n";
            } else {
                $strValue = addslashes($value);
                $code .= "\tzend_string *{$zvalName}_str = zend_string_init(\"$strValue\", sizeof(\"$strValue\") - 1, 1);\n";
                $code .= "\tZVAL_STR(&$zvalName, {$zvalName}_str);\n";
            }
            break;

        case "array":
            if (empty($value)) {
                $code .= "\tZVAL_EMPTY_ARRAY(&$zvalName);\n";
            } else {
                throw new Exception("Unimplemented default value");
            }
            break;

        default:
            throw new Exception("Invalid default value");
    }

    return $code;
}

class PropertyInfo
{
    /** @var PropertyName */
    public $name;
    /** @var int */
    public $flags;
    /** @var Type|null */
    public $type;
    /** @var Type|null */
    public $phpDocType;
    /** @var Expr|null */
    public $defaultValue;
    /** @var string|null */
    public $defaultValueString;
    /** @var bool */
    public $isDocReadonly;
    /** @var string|null */
    public $link;

    public function __construct(
        PropertyName $name,
        int $flags,
        ?Type $type,
        ?Type $phpDocType,
        ?Expr $defaultValue,
        ?string $defaultValueString,
        bool $isDocReadonly,
        ?string $link
    ) {
        $this->name = $name;
        $this->flags = $flags;
        $this->type = $type;
        $this->phpDocType = $phpDocType;
        $this->defaultValue = $defaultValue;
        $this->defaultValueString = $defaultValueString;
        $this->isDocReadonly = $isDocReadonly;
        $this->link = $link;
    }

    public function discardInfoForOldPhpVersions(): void {
        $this->type = null;
    }

    public function getDeclaration(): string {
        $code = "\n";

        $propertyName = $this->name->property;

        $defaultValueConstant = false;
        if ($this->defaultValue === null) {
            $defaultValue = null;
        } else {
            $defaultValue = $this->evaluateDefaultValue($defaultValueConstant);
        }

        if ($defaultValueConstant) {
            echo "Skipping code generation for property $this->name, because it has a constant default value\n";
            return "";
        }

        $typeCode = "";
        if ($this->type) {
            $arginfoType = $this->type->toArginfoType();
            if ($arginfoType->hasClassType()) {
                if (count($arginfoType->classTypes) >= 2) {
                    foreach ($arginfoType->classTypes as $classType) {
                        $escapedClassName = $classType->toEscapedName();
                        $varEscapedClassName = $classType->toVarEscapedName();
                        $code .= "\tzend_string *property_{$propertyName}_class_{$varEscapedClassName} = zend_string_init(\"{$escapedClassName}\", sizeof(\"{$escapedClassName}\") - 1, 1);\n";
                    }

                    $classTypeCount = count($arginfoType->classTypes);
                    $code .= "\tzend_type_list *property_{$propertyName}_type_list = malloc(ZEND_TYPE_LIST_SIZE($classTypeCount));\n";
                    $code .= "\tproperty_{$propertyName}_type_list->num_types = $classTypeCount;\n";

                    foreach ($arginfoType->classTypes as $k => $classType) {
                        $escapedClassName = $classType->toEscapedName();
                        $code .= "\tproperty_{$propertyName}_type_list->types[$k] = (zend_type) ZEND_TYPE_INIT_CLASS(property_{$propertyName}_class_{$escapedClassName}, 0, 0);\n";
                    }

                    $typeMaskCode = $this->type->toArginfoType()->toTypeMask();

                    $code .= "\tzend_type property_{$propertyName}_type = ZEND_TYPE_INIT_UNION(property_{$propertyName}_type_list, $typeMaskCode);\n";
                    $typeCode = "property_{$propertyName}_type";
                } else {
                    $escapedClassName = $arginfoType->classTypes[0]->toEscapedName();
                    $varEscapedClassName = $arginfoType->classTypes[0]->toVarEscapedName();
                    $code .= "\tzend_string *property_{$propertyName}_class_{$varEscapedClassName} = zend_string_init(\"{$escapedClassName}\", sizeof(\"${escapedClassName}\")-1, 1);\n";

                    $typeCode = "(zend_type) ZEND_TYPE_INIT_CLASS(property_{$propertyName}_class_{$varEscapedClassName}, 0, " . $arginfoType->toTypeMask() . ")";
                }
            } else {
                $typeCode = "(zend_type) ZEND_TYPE_INIT_MASK(" . $arginfoType->toTypeMask() . ")";
            }
        }

        $zvalName = "property_{$this->name->property}_default_value";
        if ($this->defaultValue === null && $this->type !== null) {
            $code .= "\tzval $zvalName;\n\tZVAL_UNDEF(&$zvalName);\n";
        } else {
            $code .= initializeZval($zvalName, $defaultValue);
        }

        $code .= "\tzend_string *property_{$propertyName}_name = zend_string_init(\"$propertyName\", sizeof(\"$propertyName\") - 1, 1);\n";
        $nameCode = "property_{$propertyName}_name";

        if ($this->type !== null) {
            $code .= "\tzend_declare_typed_property(class_entry, $nameCode, &$zvalName, " . $this->getFlagsAsString() . ", NULL, $typeCode);\n";
        } else {
            $code .= "\tzend_declare_property_ex(class_entry, $nameCode, &$zvalName, " . $this->getFlagsAsString() . ", NULL);\n";
        }
        $code .= "\tzend_string_release(property_{$propertyName}_name);\n";

        return $code;
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

        if ($this->flags & Class_::MODIFIER_READONLY) {
            $flags .= "|ZEND_ACC_READONLY";
        }

        return $flags;
    }

    public function getFieldSynopsisElement(DOMDocument $doc): DOMElement
    {
        $fieldsynopsisElement = $doc->createElement("fieldsynopsis");

        if ($this->flags & Class_::MODIFIER_PUBLIC) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "public"));
        } elseif ($this->flags & Class_::MODIFIER_PROTECTED) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "protected"));
        } elseif ($this->flags & Class_::MODIFIER_PRIVATE) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "private"));
        }

        if ($this->flags & Class_::MODIFIER_STATIC) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "static"));
        } elseif ($this->flags & Class_::MODIFIER_READONLY || $this->isDocReadonly) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "readonly"));
        }

        $fieldsynopsisElement->appendChild(new DOMText("\n     "));
        $fieldsynopsisElement->appendChild($this->getFieldSynopsisType()->getTypeForDoc($doc));

        $className = str_replace(["\\", "_"], ["-", "-"], $this->name->class->toLowerString());
        $varnameElement = $doc->createElement("varname", $this->name->property);
        if ($this->link) {
            $varnameElement->setAttribute("linkend", $this->link);
        } else {
            $varnameElement->setAttribute("linkend", "$className.props." . strtolower(str_replace("_", "-", $this->name->property)));
        }
        $fieldsynopsisElement->appendChild(new DOMText("\n     "));
        $fieldsynopsisElement->appendChild($varnameElement);

        if ($this->defaultValueString) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $initializerElement = $doc->createElement("initializer",  $this->defaultValueString);
            $fieldsynopsisElement->appendChild($initializerElement);
        }

        $fieldsynopsisElement->appendChild(new DOMText("\n    "));

        return $fieldsynopsisElement;
    }

    private function getFieldSynopsisType(): Type {
        if ($this->phpDocType) {
            return $this->phpDocType;
        }

        if ($this->type) {
            return $this->type;
        }

        throw new Exception("A property must have a type");
    }

    /** @return mixed */
    private function evaluateDefaultValue(bool &$defaultValueConstant)
    {
        $evaluator = new ConstExprEvaluator(
            function (Expr $expr) use (&$defaultValueConstant) {
                if ($expr instanceof Expr\ConstFetch) {
                    $defaultValueConstant = true;
                    return null;
                }

                throw new Exception("Property $this->name has an unsupported default value");
            }
        );

        return $evaluator->evaluateDirectly($this->defaultValue);
    }

    public function __clone()
    {
        if ($this->type) {
            $this->type = clone $this->type;
        }
    }
}

class EnumCaseInfo {
    /** @var string */
    public $name;
    /** @var Expr|null */
    public $value;

    public function __construct(string $name, ?Expr $value) {
        $this->name = $name;
        $this->value = $value;
    }

    public function getDeclaration(): string {
        $escapedName = addslashes($this->name);
        if ($this->value === null) {
            $code = "\n\tzend_enum_add_case_cstr(class_entry, \"$escapedName\", NULL);\n";
        } else {
            $evaluator = new ConstExprEvaluator(function (Expr $expr) {
                throw new Exception("Enum case $this->name has an unsupported value");
            });
            $zvalName = "enum_case_{$escapedName}_value";
            $code = "\n" . initializeZval($zvalName, $evaluator->evaluateDirectly($this->value));
            $code .= "\tzend_enum_add_case_cstr(class_entry, \"$escapedName\", &$zvalName);\n";
        }
        return $code;
    }
}

class ClassInfo {
    /** @var Name */
    public $name;
    /** @var int */
    public $flags;
    /** @var string */
    public $type;
    /** @var string|null */
    public $alias;
    /** @var SimpleType|null */
    public $enumBackingType;
    /** @var bool */
    public $isDeprecated;
    /** @var bool */
    public $isStrictProperties;
    /** @var bool */
    public $isNotSerializable;
    /** @var Name[] */
    public $extends;
    /** @var Name[] */
    public $implements;
    /** @var PropertyInfo[] */
    public $propertyInfos;
    /** @var FuncInfo[] */
    public $funcInfos;
    /** @var EnumCaseInfo[] */
    public $enumCaseInfos;

    /**
     * @param Name[] $extends
     * @param Name[] $implements
     * @param PropertyInfo[] $propertyInfos
     * @param FuncInfo[] $funcInfos
     * @param EnumCaseInfo[] $enumCaseInfos
     */
    public function __construct(
        Name $name,
        int $flags,
        string $type,
        ?string $alias,
        ?SimpleType $enumBackingType,
        bool $isDeprecated,
        bool $isStrictProperties,
        bool $isNotSerializable,
        array $extends,
        array $implements,
        array $propertyInfos,
        array $funcInfos,
        array $enumCaseInfos
    ) {
        $this->name = $name;
        $this->flags = $flags;
        $this->type = $type;
        $this->alias = $alias;
        $this->enumBackingType = $enumBackingType;
        $this->isDeprecated = $isDeprecated;
        $this->isStrictProperties = $isStrictProperties;
        $this->isNotSerializable = $isNotSerializable;
        $this->extends = $extends;
        $this->implements = $implements;
        $this->propertyInfos = $propertyInfos;
        $this->funcInfos = $funcInfos;
        $this->enumCaseInfos = $enumCaseInfos;
    }

    public function getRegistration(): string
    {
        $params = [];
        foreach ($this->extends as $extends) {
            $params[] = "zend_class_entry *class_entry_" . implode("_", $extends->parts);
        }
        foreach ($this->implements as $implements) {
            $params[] = "zend_class_entry *class_entry_" . implode("_", $implements->parts);
        }

        $escapedName = implode("_", $this->name->parts);

        $code = "static zend_class_entry *register_class_$escapedName(" . (empty($params) ? "void" : implode(", ", $params)) . ")\n";

        $code .= "{\n";
        if ($this->type == "enum") {
            $name = addslashes((string) $this->name);
            $backingType = $this->enumBackingType
                ? $this->enumBackingType->toTypeCode() : "IS_UNDEF";
            $code .= "\tzend_class_entry *class_entry = zend_register_internal_enum(\"$name\", $backingType, class_{$escapedName}_methods);\n";
        } else {
            $code .= "\tzend_class_entry ce, *class_entry;\n\n";
            if (count($this->name->parts) > 1) {
                $className = $this->name->getLast();
                $namespace = addslashes((string) $this->name->slice(0, -1));

                $code .= "\tINIT_NS_CLASS_ENTRY(ce, \"$namespace\", \"$className\", class_{$escapedName}_methods);\n";
            } else {
                $code .= "\tINIT_CLASS_ENTRY(ce, \"$this->name\", class_{$escapedName}_methods);\n";
            }

            if ($this->type === "class" || $this->type === "trait") {
                $code .= "\tclass_entry = zend_register_internal_class_ex(&ce, " . (isset($this->extends[0]) ? "class_entry_" . str_replace("\\", "_", $this->extends[0]->toString()) : "NULL") . ");\n";
            } else {
                $code .= "\tclass_entry = zend_register_internal_interface(&ce);\n";
            }
        }

        if ($this->getFlagsAsString()) {
            $code .= "\tclass_entry->ce_flags |= " . $this->getFlagsAsString() . ";\n";
        }

        $implements = array_map(
            function (Name $item) {
                return "class_entry_" . implode("_", $item->parts);
            },
            $this->type === "interface" ? $this->extends : $this->implements
        );

        if (!empty($implements)) {
            $code .= "\tzend_class_implements(class_entry, " . count($implements) . ", " . implode(", ", $implements) . ");\n";
        }

        if ($this->alias) {
            $code .= "\tzend_register_class_alias(\"" . str_replace("\\", "\\\\", $this->alias) . "\", class_entry);\n";
        }

        foreach ($this->enumCaseInfos as $enumCase) {
            $code .= $enumCase->getDeclaration();
        }

        foreach ($this->propertyInfos as $property) {
            $code .= $property->getDeclaration();
        }

        $code .= "\n\treturn class_entry;\n";

        $code .= "}\n";

        return $code;
    }

    private function getFlagsAsString(): string
    {
        $flags = [];

        if ($this->type === "trait") {
            $flags[] = "ZEND_ACC_TRAIT";
        }

        if ($this->flags & Class_::MODIFIER_FINAL) {
            $flags[] = "ZEND_ACC_FINAL";
        }

        if ($this->flags & Class_::MODIFIER_ABSTRACT) {
            $flags[] = "ZEND_ACC_ABSTRACT";
        }

        if ($this->isDeprecated) {
            $flags[] = "ZEND_ACC_DEPRECATED";
        }

        if ($this->isStrictProperties) {
            $flags[] = "ZEND_ACC_NO_DYNAMIC_PROPERTIES";
        }

        if ($this->isNotSerializable) {
            $flags[] = "ZEND_ACC_NOT_SERIALIZABLE";
        }

        return implode("|", $flags);
    }

    /**
     * @param array<string, ClassInfo> $classMap
     */
    public function getClassSynopsisDocument(array $classMap): ?string {

        $doc = new DOMDocument();
        $doc->formatOutput = true;
        $classSynopsis = $this->getClassSynopsisElement($doc, $classMap);
        if (!$classSynopsis) {
            return null;
        }

        $doc->appendChild($classSynopsis);

        return $doc->saveXML();
    }

    /**
     * @param ClassInfo[] $classMap
     */
    public function getClassSynopsisElement(DOMDocument $doc, array $classMap): ?DOMElement {

        $classSynopsis = $doc->createElement("classsynopsis");
        $classSynopsis->appendChild(new DOMText("\n    "));

        $ooElement = self::createOoElement($doc, $this, true, false, false, 4);
        if (!$ooElement) {
            return null;
        }
        $classSynopsis->appendChild($ooElement);
        $classSynopsis->appendChild(new DOMText("\n\n    "));

        $classSynopsisInfo = $doc->createElement("classsynopsisinfo");
        $classSynopsisInfo->appendChild(new DOMText("\n     "));
        $ooElement = self::createOoElement($doc, $this, false, true, false, 5);
        if (!$ooElement) {
            return null;
        }
        $classSynopsisInfo->appendChild($ooElement);

        $classSynopsis->appendChild($classSynopsisInfo);

        foreach ($this->extends as $k => $parent) {
            $parentInfo = $classMap[$parent->toString()] ?? null;
            if ($parentInfo === null) {
                throw new Exception("Missing parent class " . $parent->toString());
            }

            $ooElement = self::createOoElement(
                $doc,
                $parentInfo,
                $this->type === "interface",
                false,
                $k === 0,
                5
            );
            if (!$ooElement) {
                return null;
            }

            $classSynopsisInfo->appendChild(new DOMText("\n\n     "));
            $classSynopsisInfo->appendChild($ooElement);
        }

        foreach ($this->implements as $interface) {
            $interfaceInfo = $classMap[$interface->toString()] ?? null;
            if (!$interfaceInfo) {
                throw new Exception("Missing implemented interface " . $interface->toString());
            }

            $ooElement = self::createOoElement($doc, $interfaceInfo, false, false, false, 5);
            if (!$ooElement) {
                return null;
            }
            $classSynopsisInfo->appendChild(new DOMText("\n\n     "));
            $classSynopsisInfo->appendChild($ooElement);
        }
        $classSynopsisInfo->appendChild(new DOMText("\n    "));

        /** @var Name[] $parentsWithInheritedProperties */
        $parentsWithInheritedProperties = [];
        /** @var Name[] $parentsWithInheritedMethods */
        $parentsWithInheritedMethods = [];

        $this->collectInheritedMembers($parentsWithInheritedProperties, $parentsWithInheritedMethods, $classMap);

        if (!empty($this->propertyInfos)) {
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "&Properties;");
            $classSynopsisInfo->setAttribute("role", "comment");
            $classSynopsis->appendChild($classSynopsisInfo);

            foreach ($this->propertyInfos as $propertyInfo) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $fieldSynopsisElement = $propertyInfo->getFieldSynopsisElement($doc);
                $classSynopsis->appendChild($fieldSynopsisElement);
            }
        }

        if (!empty($parentsWithInheritedProperties)) {
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "&InheritedProperties;");
            $classSynopsisInfo->setAttribute("role", "comment");
            $classSynopsis->appendChild($classSynopsisInfo);

            foreach ($parentsWithInheritedProperties as $parent) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $parentReference = self::getClassSynopsisReference($parent);

                $includeElement = $this->createIncludeElement(
                    $doc,
                    "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$parentReference')/db:partintro/db:section/db:classsynopsis/db:fieldsynopsis[preceding-sibling::db:classsynopsisinfo[1][@role='comment' and text()='&Properties;']]))"
                );
                $classSynopsis->appendChild($includeElement);
            }
        }

        if (!empty($this->funcInfos)) {
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "&Methods;");
            $classSynopsisInfo->setAttribute("role", "comment");
            $classSynopsis->appendChild($classSynopsisInfo);

            $classReference = self::getClassSynopsisReference($this->name);

            if ($this->hasConstructor()) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $includeElement = $this->createIncludeElement(
                    $doc,
                    "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$classReference')/db:refentry/db:refsect1[@role='description']/descendant::db:constructorsynopsis[not(@role='procedural')])"
                );
                $classSynopsis->appendChild($includeElement);
            }

            if ($this->hasMethods()) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $includeElement = $this->createIncludeElement(
                    $doc,
                    "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$classReference')/db:refentry/db:refsect1[@role='description']/descendant::db:methodsynopsis[not(@role='procedural')])"
                );
                $classSynopsis->appendChild($includeElement);
            }

            if ($this->hasDestructor()) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $includeElement = $this->createIncludeElement(
                    $doc,
                    "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$classReference')/db:refentry/db:refsect1[@role='description']/descendant::db:destructorsynopsis[not(@role='procedural')])"
                );
                $classSynopsis->appendChild($includeElement);
            }
        }

        if (!empty($parentsWithInheritedMethods)) {
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "&InheritedMethods;");
            $classSynopsisInfo->setAttribute("role", "comment");
            $classSynopsis->appendChild($classSynopsisInfo);

            foreach ($parentsWithInheritedMethods as $parent) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $parentReference = self::getClassSynopsisReference($parent);
                $includeElement = $this->createIncludeElement(
                    $doc,
                    "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$parentReference')/db:refentry/db:refsect1[@role='description']/descendant::db:methodsynopsis[not(@role='procedural')])"
                );
                $classSynopsis->appendChild($includeElement);
            }
        }

        $classSynopsis->appendChild(new DOMText("\n   "));

        return $classSynopsis;
    }

    private static function createOoElement(
        DOMDocument $doc,
        ClassInfo $classInfo,
        bool $overrideToClass,
        bool $withModifiers,
        bool $isExtends,
        int $indentationLevel
    ): ?DOMElement {
        $indentation = str_repeat(" ", $indentationLevel);

        if ($classInfo->type !== "class" && $classInfo->type !== "interface") {
            echo "Class synopsis generation is not implemented for " . $classInfo->type . "\n";
            return null;
        }

        $type = $overrideToClass ? "class" : $classInfo->type;

        $ooElement = $doc->createElement("oo$type");
        $ooElement->appendChild(new DOMText("\n$indentation "));
        if ($isExtends) {
            $ooElement->appendChild($doc->createElement('modifier', 'extends'));
            $ooElement->appendChild(new DOMText("\n$indentation "));
        } elseif ($withModifiers) {
            if ($classInfo->flags & Class_::MODIFIER_FINAL) {
                $ooElement->appendChild($doc->createElement('modifier', 'final'));
                $ooElement->appendChild(new DOMText("\n$indentation "));
            }
            if ($classInfo->flags & Class_::MODIFIER_ABSTRACT) {
                $ooElement->appendChild($doc->createElement('modifier', 'abstract'));
                $ooElement->appendChild(new DOMText("\n$indentation "));
            }
        }

        $nameElement = $doc->createElement("{$type}name", $classInfo->name->toString());
        $ooElement->appendChild($nameElement);
        $ooElement->appendChild(new DOMText("\n$indentation"));

        return $ooElement;
    }

    public static function getClassSynopsisFilename(Name $name): string {
        return strtolower(str_replace("_", "-", implode('-', $name->parts)));
    }

    public static function getClassSynopsisReference(Name $name): string {
        return "class." . self::getClassSynopsisFilename($name);
    }

    /**
     * @param Name[] $parentsWithInheritedProperties
     * @param Name[] $parentsWithInheritedMethods
     * @param array<string, ClassInfo> $classMap
     */
    private function collectInheritedMembers(array &$parentsWithInheritedProperties, array &$parentsWithInheritedMethods, array $classMap): void
    {
        foreach ($this->extends as $parent) {
            $parentInfo = $classMap[$parent->toString()] ?? null;
            if (!$parentInfo) {
                throw new Exception("Missing parent class " . $parent->toString());
            }

            if (!empty($parentInfo->propertyInfos) && !isset($parentsWithInheritedProperties[$parent->toString()])) {
                $parentsWithInheritedProperties[$parent->toString()] = $parent;
            }

            if (!isset($parentsWithInheritedMethods[$parent->toString()]) && $parentInfo->hasMethods()) {
                $parentsWithInheritedMethods[$parent->toString()] = $parent;
            }

            $parentInfo->collectInheritedMembers($parentsWithInheritedProperties, $parentsWithInheritedMethods, $classMap);
        }
    }

    private function hasConstructor(): bool
    {
        foreach ($this->funcInfos as $funcInfo) {
            if ($funcInfo->name->isConstructor()) {
                return true;
            }
        }

        return false;
    }

    private function hasDestructor(): bool
    {
        foreach ($this->funcInfos as $funcInfo) {
            if ($funcInfo->name->isDestructor()) {
                return true;
            }
        }

        return false;
    }

    private function hasMethods(): bool
    {
        foreach ($this->funcInfos as $funcInfo) {
            if (!$funcInfo->name->isConstructor() && !$funcInfo->name->isDestructor()) {
                return true;
            }
        }

        return false;
    }

    private function createIncludeElement(DOMDocument $doc, string $query): DOMElement
    {
        $includeElement = $doc->createElement("xi:include");
        $attr = $doc->createAttribute("xpointer");
        $attr->value = $query;
        $includeElement->appendChild($attr);
        $fallbackElement = $doc->createElement("xi:fallback");
        $includeElement->appendChild(new DOMText("\n     "));
        $includeElement->appendChild($fallbackElement);
        $includeElement->appendChild(new DOMText("\n    "));

        return $includeElement;
    }

    public function __clone()
    {
        foreach ($this->propertyInfos as $key => $propertyInfo) {
            $this->propertyInfos[$key] = clone $propertyInfo;
        }

        foreach ($this->funcInfos as $key => $funcInfo) {
            $this->funcInfos[$key] = clone $funcInfo;
        }
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
    /** @var bool */
    public $generateClassEntries = false;

    /**
     * @return iterable<FuncInfo>
     */
    public function getAllFuncInfos(): iterable {
        yield from $this->funcInfos;
        foreach ($this->classInfos as $classInfo) {
            yield from $classInfo->funcInfos;
        }
    }

    /**
     * @return iterable<PropertyInfo>
     */
    public function getAllPropertyInfos(): iterable {
        foreach ($this->classInfos as $classInfo) {
            yield from $classInfo->propertyInfos;
        }
    }

    public function __clone()
    {
        foreach ($this->funcInfos as $key => $funcInfo) {
            $this->funcInfos[$key] = clone $funcInfo;
        }

        foreach ($this->classInfos as $key => $classInfo) {
            $this->classInfos[$key] = clone $classInfo;
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

    public function getType(): string {
        $value = $this->getValue();

        $matches = [];

        if ($this->name === "param") {
            preg_match('/^\s*([\w\|\\\\\[\]<>, ]+)\s*\$\w+.*$/', $value, $matches);
        } elseif ($this->name === "return" || $this->name === "var") {
            preg_match('/^\s*([\w\|\\\\\[\]<>, ]+)/', $value, $matches);
        }

        if (!isset($matches[1])) {
            throw new Exception("@$this->name doesn't contain a type or has an invalid format \"$value\"");
        }

        return trim($matches[1]);
    }

    public function getVariableName(): string {
        $value = $this->value;
        if ($value === null || strlen($value) === 0) {
            throw new Exception("@$this->name doesn't have any value");
        }

        $matches = [];

        if ($this->name === "param") {
            preg_match('/^\s*[\w\|\\\\\[\]]+\s*\$(\w+).*$/', $value, $matches);
        } elseif ($this->name === "prefer-ref") {
            preg_match('/^\s*\$(\w+).*$/', $value, $matches);
        }

        if (!isset($matches[1])) {
            throw new Exception("@$this->name doesn't contain a variable name or has an invalid format \"$value\"");
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
    try {
        $comment = $func->getDocComment();
        $paramMeta = [];
        $aliasType = null;
        $alias = null;
        $isDeprecated = false;
        $verify = true;
        $docReturnType = null;
        $tentativeReturnType = false;
        $docParamTypes = [];
        $refcount = null;

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
                } else if ($tag->name === 'no-verify') {
                    $verify = false;
                } else if ($tag->name === 'tentative-return-type') {
                    $tentativeReturnType = true;
                } else if ($tag->name === 'return') {
                    $docReturnType = $tag->getType();
                } else if ($tag->name === 'param') {
                    $docParamTypes[$tag->getVariableName()] = $tag->getType();
                } else if ($tag->name === 'refcount') {
                    $refcount = $tag->getValue();
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
                throw new Exception("Duplicate parameter name $varName");
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
                throw new Exception("Only the last parameter can be variadic");
            }

            $type = $param->type ? Type::fromNode($param->type) : null;
            if ($type === null && !isset($docParamTypes[$varName])) {
                throw new Exception("Missing parameter type");
            }

            if ($param->default instanceof Expr\ConstFetch &&
                $param->default->name->toLowerString() === "null" &&
                $type && !$type->isNullable()
            ) {
                $simpleType = $type->tryToSimpleType();
                if ($simpleType === null) {
                    throw new Exception("Parameter $varName has null default, but is not nullable");
                }
            }

            if ($param->default instanceof Expr\ClassConstFetch && $param->default->class->toLowerString() === "self") {
                throw new Exception('The exact class name must be used instead of "self"');
            }

            $foundVariadic = $param->variadic;

            $args[] = new ArgInfo(
                $varName,
                $sendBy,
                $param->variadic,
                $type,
                isset($docParamTypes[$varName]) ? Type::fromString($docParamTypes[$varName]) : null,
                $param->default ? $prettyPrinter->prettyPrintExpr($param->default) : null
            );
            if (!$param->default && !$param->variadic) {
                $numRequiredArgs = $i + 1;
            }
        }

        foreach (array_keys($paramMeta) as $var) {
            throw new Exception("Found metadata for invalid param $var");
        }

        $returnType = $func->getReturnType();
        if ($returnType === null && $docReturnType === null && !$name->isConstructor() && !$name->isDestructor()) {
            throw new Exception("Missing return type");
        }

        $return = new ReturnInfo(
            $func->returnsByRef(),
            $returnType ? Type::fromNode($returnType) : null,
            $docReturnType ? Type::fromString($docReturnType) : null,
            $tentativeReturnType,
            $refcount
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
    } catch (Exception $e) {
        throw new Exception($name . "(): " .$e->getMessage());
    }
}

function parseProperty(
    Name $class,
    int $flags,
    Stmt\PropertyProperty $property,
    ?Node $type,
    ?DocComment $comment,
    PrettyPrinterAbstract $prettyPrinter
): PropertyInfo {
    $phpDocType = null;
    $isDocReadonly = false;
    $link = null;

    if ($comment) {
        $tags = parseDocComment($comment);
        foreach ($tags as $tag) {
            if ($tag->name === 'var') {
                $phpDocType = $tag->getType();
            } elseif ($tag->name === 'readonly') {
                $isDocReadonly = true;
            } elseif ($tag->name === 'link') {
                $link = $tag->value;
            }
        }
    }

    $propertyType = $type ? Type::fromNode($type) : null;
    if ($propertyType === null && !$phpDocType) {
        throw new Exception("Missing type for property $class::\$$property->name");
    }

    if ($property->default instanceof Expr\ConstFetch &&
        $property->default->name->toLowerString() === "null" &&
        $propertyType && !$propertyType->isNullable()
    ) {
        $simpleType = $propertyType->tryToSimpleType();
        if ($simpleType === null) {
            throw new Exception(
                "Property $class::\$$property->name has null default, but is not nullable");
        }
    }

    return new PropertyInfo(
        new PropertyName($class, $property->name->__toString()),
        $flags,
        $propertyType,
        $phpDocType ? Type::fromString($phpDocType) : null,
        $property->default,
        $property->default ? $prettyPrinter->prettyPrintExpr($property->default) : null,
        $isDocReadonly,
        $link
    );
}

/**
 * @param PropertyInfo[] $properties
 * @param FuncInfo[] $methods
 * @param EnumCaseInfo[] $enumCases
 */
function parseClass(
    Name $name, Stmt\ClassLike $class, array $properties, array $methods, array $enumCases
): ClassInfo {
    $flags = $class instanceof Class_ ? $class->flags : 0;
    $comment = $class->getDocComment();
    $alias = null;
    $isDeprecated = false;
    $isStrictProperties = false;
    $isNotSerializable = false;

    if ($comment) {
        $tags = parseDocComment($comment);
        foreach ($tags as $tag) {
            if ($tag->name === 'alias') {
                $alias = $tag->getValue();
            } else if ($tag->name === 'deprecated') {
                $isDeprecated = true;
            } else if ($tag->name === 'strict-properties') {
                $isStrictProperties = true;
            } else if ($tag->name === 'not-serializable') {
                $isNotSerializable = true;
            }
        }
    }

    $extends = [];
    $implements = [];

    if ($class instanceof Class_) {
        $classKind = "class";
        if ($class->extends) {
            $extends[] = $class->extends;
        }
        $implements = $class->implements;
    } elseif ($class instanceof Interface_) {
        $classKind = "interface";
        $extends = $class->extends;
    } else if ($class instanceof Trait_) {
        $classKind = "trait";
    } else if ($class instanceof Enum_) {
        $classKind = "enum";
        $implements = $class->implements;
    } else {
        throw new Exception("Unknown class kind " . get_class($class));
    }

    return new ClassInfo(
        $name,
        $flags,
        $classKind,
        $alias,
        $class instanceof Enum_ && $class->scalarType !== null
            ? SimpleType::fromNode($class->scalarType) : null,
        $isDeprecated,
        $isStrictProperties,
        $isNotSerializable,
        $extends,
        $implements,
        $properties,
        $methods,
        $enumCases
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
            $propertyInfos = [];
            $methodInfos = [];
            $enumCaseInfos = [];
            foreach ($stmt->stmts as $classStmt) {
                $cond = handlePreprocessorConditions($conds, $classStmt);
                if ($classStmt instanceof Stmt\Nop) {
                    continue;
                }

                $classFlags = $stmt instanceof Class_ ? $stmt->flags : 0;
                $abstractFlag = $stmt instanceof Stmt\Interface_ ? Class_::MODIFIER_ABSTRACT : 0;

                if ($classStmt instanceof Stmt\Property) {
                    if (!($classStmt->flags & Class_::VISIBILITY_MODIFIER_MASK)) {
                        throw new Exception("Visibility modifier is required");
                    }
                    foreach ($classStmt->props as $property) {
                        $propertyInfos[] = parseProperty(
                            $className,
                            $classStmt->flags,
                            $property,
                            $classStmt->type,
                            $classStmt->getDocComment(),
                            $prettyPrinter
                        );
                    }
                } else if ($classStmt instanceof Stmt\ClassMethod) {
                    if (!($classStmt->flags & Class_::VISIBILITY_MODIFIER_MASK)) {
                        throw new Exception("Visibility modifier is required");
                    }
                    $methodInfos[] = parseFunctionLike(
                        $prettyPrinter,
                        new MethodName($className, $classStmt->name->toString()),
                        $classFlags,
                        $classStmt->flags | $abstractFlag,
                        $classStmt,
                        $cond
                    );
                } else if ($classStmt instanceof Stmt\EnumCase) {
                    $enumCaseInfos[] = new EnumCaseInfo(
                        $classStmt->name->toString(), $classStmt->expr);
                } else {
                    throw new Exception("Not implemented {$classStmt->getType()}");
                }
            }

            $fileInfo->classInfos[] = parseClass(
                $className, $stmt, $propertyInfos, $methodInfos, $enumCaseInfos);
            continue;
        }

        throw new Exception("Unexpected node {$stmt->getType()}");
    }
}

function parseStubFile(string $code): FileInfo {
    $lexer = new PhpParser\Lexer\Emulative();
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
            } else if ($tag->name === 'generate-class-entries') {
                $fileInfo->generateClassEntries = true;
                $fileInfo->declarationPrefix = $tag->value ? $tag->value . " " : "";
            }
        }
    }

    // Generating class entries require generating function/method entries
    if ($fileInfo->generateClassEntries && !$fileInfo->generateFunctionEntries) {
        $fileInfo->generateFunctionEntries = true;
    }

    handleStatements($fileInfo, $stmts, $prettyPrinter);
    return $fileInfo;
}

function funcInfoToCode(FuncInfo $funcInfo): string {
    $code = '';
    $returnType = $funcInfo->return->type;
    $isTentativeReturnType = $funcInfo->return->tentativeReturnType;

    if ($returnType !== null) {
        if (null !== $simpleReturnType = $returnType->tryToSimpleType()) {
            if ($simpleReturnType->isBuiltin) {
                $code .= sprintf(
                    "%s(%s, %d, %d, %s, %d)\n",
                    $isTentativeReturnType ? "ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX" : "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $simpleReturnType->toTypeCode(), $returnType->isNullable()
                );
            } else {
                $code .= sprintf(
                    "%s(%s, %d, %d, %s, %d)\n",
                    $isTentativeReturnType ? "ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX" : "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $simpleReturnType->toEscapedName(), $returnType->isNullable()
                );
            }
        } else {
            $arginfoType = $returnType->toArginfoType();
            if ($arginfoType->hasClassType()) {
                $code .= sprintf(
                    "%s(%s, %d, %d, %s, %s)\n",
                    $isTentativeReturnType ? "ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX" : "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $arginfoType->toClassTypeString(), $arginfoType->toTypeMask()
                );
            } else {
                $code .= sprintf(
                    "%s(%s, %d, %d, %s)\n",
                    $isTentativeReturnType ? "ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX" : "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $arginfoType->toTypeMask()
                );
            }
        }
    } else {
        $code .= sprintf(
            "ZEND_BEGIN_ARG_INFO_EX(%s, 0, %d, %d)\n",
            $funcInfo->getArgInfoName(), $funcInfo->return->byRef, $funcInfo->numRequiredArgs
        );
    }

    foreach ($funcInfo->args as $argInfo) {
        $argKind = $argInfo->isVariadic ? "ARG_VARIADIC" : "ARG";
        $argDefaultKind = $argInfo->hasProperDefaultValue() ? "_WITH_DEFAULT_VALUE" : "";
        $argType = $argInfo->type;
        if ($argType !== null) {
            if (null !== $simpleArgType = $argType->tryToSimpleType()) {
                if ($simpleArgType->isBuiltin) {
                    $code .= sprintf(
                        "\tZEND_%s_TYPE_INFO%s(%s, %s, %s, %d%s)\n",
                        $argKind, $argDefaultKind, $argInfo->getSendByString(), $argInfo->name,
                        $simpleArgType->toTypeCode(), $argType->isNullable(),
                        $argInfo->hasProperDefaultValue() ? ", " . $argInfo->getDefaultValueAsArginfoString() : ""
                    );
                } else {
                    $code .= sprintf(
                        "\tZEND_%s_OBJ_INFO%s(%s, %s, %s, %d%s)\n",
                        $argKind,$argDefaultKind, $argInfo->getSendByString(), $argInfo->name,
                        $simpleArgType->toEscapedName(), $argType->isNullable(),
                        $argInfo->hasProperDefaultValue() ? ", " . $argInfo->getDefaultValueAsArginfoString() : ""
                    );
                }
            } else {
                $arginfoType = $argType->toArginfoType();
                if ($arginfoType->hasClassType()) {
                    $code .= sprintf(
                        "\tZEND_%s_OBJ_TYPE_MASK(%s, %s, %s, %s, %s)\n",
                        $argKind, $argInfo->getSendByString(), $argInfo->name,
                        $arginfoType->toClassTypeString(), $arginfoType->toTypeMask(),
                        $argInfo->getDefaultValueAsArginfoString()
                    );
                } else {
                    $code .= sprintf(
                        "\tZEND_%s_TYPE_MASK(%s, %s, %s, %s)\n",
                        $argKind, $argInfo->getSendByString(), $argInfo->name,
                        $arginfoType->toTypeMask(),
                        $argInfo->getDefaultValueAsArginfoString()
                    );
                }
            }
        } else {
            $code .= sprintf(
                "\tZEND_%s_INFO%s(%s, %s%s)\n",
                $argKind, $argDefaultKind, $argInfo->getSendByString(), $argInfo->name,
                $argInfo->hasProperDefaultValue() ? ", " . $argInfo->getDefaultValueAsArginfoString() : ""
            );
        }
    }

    $code .= "ZEND_END_ARG_INFO()";
    return $code . "\n";
}

/** @param FuncInfo[] $generatedFuncInfos */
function findEquivalentFuncInfo(array $generatedFuncInfos, FuncInfo $funcInfo): ?FuncInfo {
    foreach ($generatedFuncInfos as $generatedFuncInfo) {
        if ($generatedFuncInfo->equalsApartFromNameAndRefcount($funcInfo)) {
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

    if ($fileInfo->generateClassEntries) {
        $code .= generateClassEntryCode($fileInfo);
    }

    return $code;
}

function generateClassEntryCode(FileInfo $fileInfo): string {
    $code = "";

    foreach ($fileInfo->classInfos as $class) {
        $code .= "\n" . $class->getRegistration();
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

/** @param FuncInfo<string, FuncInfo> $funcInfos */
function generateOptimizerInfo(array $funcInfos): string {

    $code = "/* This is a generated file, edit the .stub.php files instead. */\n\n";

    $code .= "static const func_info_t func_infos[] = {\n";

    $code .= generateCodeWithConditions($funcInfos, "", function (FuncInfo $funcInfo) {
        return $funcInfo->getOptimizerInfo();
    });

    $code .= "};\n";

    return $code;
}

/**
 * @param ClassInfo[] $classMap
 * @return array<string, string>
 */
function generateClassSynopses(array $classMap): array {
    $result = [];

    foreach ($classMap as $classInfo) {
        $classSynopsis = $classInfo->getClassSynopsisDocument($classMap);
        if ($classSynopsis !== null) {
            $result[ClassInfo::getClassSynopsisFilename($classInfo->name) . ".xml"] = $classSynopsis;
        }
    }

    return $result;
}

/**
 * @param ClassInfo[] $classMap
 * @return array<string, string>
 */
function replaceClassSynopses(string $targetDirectory, array $classMap): array
{
    $classSynopses = [];

    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($targetDirectory),
        RecursiveIteratorIterator::LEAVES_ONLY
    );

    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (!preg_match('/\.xml$/i', $pathName)) {
            continue;
        }

        $xml = file_get_contents($pathName);
        if ($xml === false) {
            continue;
        }

        if (stripos($xml, "<classsynopsis") === false) {
            continue;
        }

        $replacedXml = getReplacedSynopsisXml($xml);

        $doc = new DOMDocument();
        $doc->formatOutput = false;
        $doc->preserveWhiteSpace = true;
        $doc->validateOnParse = true;
        $success = $doc->loadXML($replacedXml);
        if (!$success) {
            echo "Failed opening $pathName\n";
            continue;
        }

        $classSynopsisElements = [];
        foreach ($doc->getElementsByTagName("classsynopsis") as $element) {
            $classSynopsisElements[] = $element;
        }

        foreach ($classSynopsisElements as $classSynopsis) {
            if (!$classSynopsis instanceof DOMElement) {
                continue;
            }

            $firstChild = $classSynopsis->firstElementChild;
            if ($firstChild === null) {
                continue;
            }
            $firstChild = $firstChild->firstElementChild;
            if ($firstChild === null) {
                continue;
            }
            $className = $firstChild->textContent;
            if (!isset($classMap[$className])) {
                continue;
            }
            $classInfo = $classMap[$className];

            $newClassSynopsis = $classInfo->getClassSynopsisElement($doc, $classMap);
            if ($newClassSynopsis === null) {
                continue;
            }

            // Check if there is any change - short circuit if there is not any.

            if (replaceAndCompareXmls($doc, $classSynopsis, $newClassSynopsis)) {
                continue;
            }

            // Return the updated XML

            $replacedXml = $doc->saveXML();

            $replacedXml = preg_replace(
                [
                    "/REPLACED-ENTITY-([A-Za-z0-9._{}%-]+?;)/",
                    "/<phpdoc:(classref|exceptionref)\s+xmlns:phpdoc=\"([a-z0-9.:\/]+)\"\s+xmlns=\"([a-z0-9.:\/]+)\"\s+xml:id=\"([a-z0-9._-]+)\"\s*>/i",
                    "/<phpdoc:(classref|exceptionref)\s+xmlns:phpdoc=\"([a-z0-9.:\/]+)\"\s+xmlns=\"([a-z0-9.:\/]+)\"\s+xmlns:xi=\"([a-z0-9.:\/]+)\"\s+xml:id=\"([a-z0-9._-]+)\"\s*>/i",
                    "/<phpdoc:(classref|exceptionref)\s+xmlns:phpdoc=\"([a-z0-9.:\/]+)\"\s+xmlns=\"([a-z0-9.:\/]+)\"\s+xmlns:xlink=\"([a-z0-9.:\/]+)\"\s+xmlns:xi=\"([a-z0-9.:\/]+)\"\s+xml:id=\"([a-z0-9._-]+)\"\s*>/i",
                    "/<phpdoc:(classref|exceptionref)\s+xmlns=\"([a-z0-9.:\/]+)\"\s+xmlns:xlink=\"([a-z0-9.:\/]+)\"\s+xmlns:xi=\"([a-z0-9.:\/]+)\"\s+xmlns:phpdoc=\"([a-z0-9.:\/]+)\"\s+xml:id=\"([a-z0-9._-]+)\"\s*>/i",
                ],
                [
                    "&$1",
                    "<phpdoc:$1 xml:id=\"$4\" xmlns:phpdoc=\"$2\" xmlns=\"$3\">",
                    "<phpdoc:$1 xml:id=\"$5\" xmlns:phpdoc=\"$2\" xmlns=\"$3\" xmlns:xi=\"$4\">",
                    "<phpdoc:$1 xml:id=\"$6\" xmlns:phpdoc=\"$2\" xmlns=\"$3\" xmlns:xlink=\"$4\" xmlns:xi=\"$5\">",
                    "<phpdoc:$1 xml:id=\"$6\" xmlns:phpdoc=\"$5\" xmlns=\"$2\" xmlns:xlink=\"$3\" xmlns:xi=\"$4\">",
                ],
                $replacedXml
            );

            $classSynopses[$pathName] = $replacedXml;
        }
    }

    return $classSynopses;
}

function getReplacedSynopsisXml(string $xml): string
{
    return preg_replace(
        [
            "/&([A-Za-z0-9._{}%-]+?;)/",
            "/<(\/)*xi:([A-Za-z]+?)/"
        ],
        [
            "REPLACED-ENTITY-$1",
            "<$1XI$2",
        ],
        $xml
    );
}

/**
 * @param array<string, FuncInfo> $funcMap
 * @param array<string, FuncInfo> $aliasMap
 * @return array<string, string>
 */
function generateMethodSynopses(array $funcMap, array $aliasMap): array {
    $result = [];

    foreach ($funcMap as $funcInfo) {
        $methodSynopsis = $funcInfo->getMethodSynopsisDocument($funcMap, $aliasMap);
        if ($methodSynopsis !== null) {
            $result[$funcInfo->name->getMethodSynopsisFilename() . ".xml"] = $methodSynopsis;
        }
    }

    return $result;
}

/**
 * @param array<string, FuncInfo> $funcMap
 * @param array<string, FuncInfo> $aliasMap
 * @return array<string, string>
 */
function replaceMethodSynopses(string $targetDirectory, array $funcMap, array $aliasMap): array {
    $methodSynopses = [];

    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($targetDirectory),
        RecursiveIteratorIterator::LEAVES_ONLY
    );

    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (!preg_match('/\.xml$/i', $pathName)) {
            continue;
        }

        $xml = file_get_contents($pathName);
        if ($xml === false) {
            continue;
        }

        if (stripos($xml, "<methodsynopsis") === false && stripos($xml, "<constructorsynopsis") === false && stripos($xml, "<destructorsynopsis") === false) {
            continue;
        }

        $replacedXml = getReplacedSynopsisXml($xml);

        $doc = new DOMDocument();
        $doc->formatOutput = false;
        $doc->preserveWhiteSpace = true;
        $doc->validateOnParse = true;
        $success = $doc->loadXML($replacedXml);
        if (!$success) {
            echo "Failed opening $pathName\n";
            continue;
        }

        $methodSynopsisElements = [];
        foreach ($doc->getElementsByTagName("constructorsynopsis") as $element) {
            $methodSynopsisElements[] = $element;
        }
        foreach ($doc->getElementsByTagName("destructorsynopsis") as $element) {
            $methodSynopsisElements[] = $element;
        }
        foreach ($doc->getElementsByTagName("methodsynopsis") as $element) {
            $methodSynopsisElements[] = $element;
        }

        foreach ($methodSynopsisElements as $methodSynopsis) {
            if (!$methodSynopsis instanceof DOMElement) {
                continue;
            }

            $list = $methodSynopsis->getElementsByTagName("methodname");
            $item = $list->item(0);
            if (!$item instanceof DOMElement) {
                continue;
            }
            $funcName = $item->textContent;
            if (!isset($funcMap[$funcName])) {
                continue;
            }
            $funcInfo = $funcMap[$funcName];

            $newMethodSynopsis = $funcInfo->getMethodSynopsisElement($funcMap, $aliasMap, $doc);
            if ($newMethodSynopsis === null) {
                continue;
            }

            // Retrieve current signature

            $params = [];
            $list = $methodSynopsis->getElementsByTagName("methodparam");
            foreach ($list as $i => $item) {
                if (!$item instanceof DOMElement) {
                    continue;
                }

                $paramList = $item->getElementsByTagName("parameter");
                if ($paramList->count() !== 1) {
                    continue;
                }

                $paramName = $paramList->item(0)->textContent;
                $paramTypes = [];

                $paramList = $item->getElementsByTagName("type");
                foreach ($paramList as $type) {
                    if (!$type instanceof DOMElement) {
                        continue;
                    }

                    $paramTypes[] = $type->textContent;
                }

                $params[$paramName] = ["index" => $i, "type" => $paramTypes];
            }

            // Check if there is any change - short circuit if there is not any.

            if (replaceAndCompareXmls($doc, $methodSynopsis, $newMethodSynopsis)) {
                continue;
            }

            // Update parameter references

            $paramList = $doc->getElementsByTagName("parameter");
            /** @var DOMElement $paramElement */
            foreach ($paramList as $paramElement) {
                if ($paramElement->parentNode && $paramElement->parentNode->nodeName === "methodparam") {
                    continue;
                }

                $name = $paramElement->textContent;
                if (!isset($params[$name])) {
                    continue;
                }

                $index = $params[$name]["index"];
                if (!isset($funcInfo->args[$index])) {
                    continue;
                }

                $paramElement->textContent = $funcInfo->args[$index]->name;
            }

            // Return the updated XML

            $replacedXml = $doc->saveXML();

            $replacedXml = preg_replace(
                [
                    "/REPLACED-ENTITY-([A-Za-z0-9._{}%-]+?;)/",
                    "/<refentry\s+xmlns=\"([a-z0-9.:\/]+)\"\s+xml:id=\"([a-z0-9._-]+)\"\s*>/i",
                    "/<refentry\s+xmlns=\"([a-z0-9.:\/]+)\"\s+xmlns:xlink=\"([a-z0-9.:\/]+)\"\s+xml:id=\"([a-z0-9._-]+)\"\s*>/i",
                ],
                [
                    "&$1",
                    "<refentry xml:id=\"$2\" xmlns=\"$1\">",
                    "<refentry xml:id=\"$3\" xmlns=\"$1\" xmlns:xlink=\"$2\">",
                ],
                $replacedXml
            );

            $methodSynopses[$pathName] = $replacedXml;
        }
    }

    return $methodSynopses;
}

function replaceAndCompareXmls(DOMDocument $doc, DOMElement $originalSynopsis, DOMElement $newSynopsis): bool
{
    $docComparator = new DOMDocument();
    $docComparator->preserveWhiteSpace = false;
    $docComparator->formatOutput = true;

    $xml1 = $doc->saveXML($originalSynopsis);
    $xml1 = getReplacedSynopsisXml($xml1);
    $docComparator->loadXML($xml1);
    $xml1 = $docComparator->saveXML();

    $originalSynopsis->parentNode->replaceChild($newSynopsis, $originalSynopsis);

    $xml2 = $doc->saveXML($newSynopsis);
    $xml2 = getReplacedSynopsisXml($xml2);

    $docComparator->loadXML($xml2);
    $xml2 = $docComparator->saveXML();

    return $xml1 === $xml2;
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
    $version = "4.13.0";
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
$options = getopt(
    "fh",
    [
        "force-regeneration", "parameter-stats", "help", "verify", "generate-classsynopses", "replace-classsynopses",
        "generate-methodsynopses", "replace-methodsynopses", "generate-optimizer-info"
    ],
    $optind
);

$context = new Context;
$printParameterStats = isset($options["parameter-stats"]);
$verify = isset($options["verify"]);
$generateClassSynopses = isset($options["generate-classsynopses"]);
$replaceClassSynopses = isset($options["replace-classsynopses"]);
$generateMethodSynopses = isset($options["generate-methodsynopses"]);
$replaceMethodSynopses = isset($options["replace-methodsynopses"]);
$generateOptimizerInfo = isset($options["generate-optimizer-info"]);
$context->forceRegeneration = isset($options["f"]) || isset($options["force-regeneration"]);
$context->forceParse = $context->forceRegeneration || $printParameterStats || $verify || $generateClassSynopses || $generateOptimizerInfo || $replaceClassSynopses || $generateMethodSynopses || $replaceMethodSynopses;

$targetSynopses = $argv[$argc - 1] ?? null;
if ($replaceClassSynopses && $targetSynopses === null) {
    die("A target class synopsis directory must be provided for.\n");
}

if ($replaceMethodSynopses && $targetSynopses === null) {
    die("A target method synopsis directory must be provided.\n");
}

if (isset($options["h"]) || isset($options["help"])) {
    die("\nusage: gen_stub.php [ -f | --force-regeneration ] [ --generate-classsynopses ] [ --replace-classsynopses ] [ --generate-methodsynopses ] [ --replace-methodsynopses ] [ --parameter-stats ] [ --verify ] [ --generate-optimizer-info ] [ -h | --help ] [ name.stub.php | directory ] [ directory ]\n\n");
}

$fileInfos = [];
$locations = array_slice($argv, $optind) ?: ['.'];
foreach (array_unique($locations) as $location) {
    if (is_file($location)) {
        // Generate single file.
        $fileInfo = processStubFile($location, $context);
        if ($fileInfo) {
            $fileInfos[] = $fileInfo;
        }
    } else if (is_dir($location)) {
        array_push($fileInfos, ...processDirectory($location, $context));
    } else {
        echo "$location is neither a file nor a directory.\n";
        exit(1);
    }
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

/** @var array<string, ClassInfo> $classMap */
$classMap = [];
/** @var array<string, FuncInfo> $funcMap */
$funcMap = [];
/** @var array<string, FuncInfo> $aliasMap */
$aliasMap = [];

foreach ($fileInfos as $fileInfo) {
    foreach ($fileInfo->getAllFuncInfos() as $funcInfo) {
        /** @var FuncInfo $funcInfo */
        $funcMap[$funcInfo->name->__toString()] = $funcInfo;

        // TODO: Don't use aliasMap for methodsynopsis?
        if ($funcInfo->aliasType === "alias") {
            $aliasMap[$funcInfo->alias->__toString()] = $funcInfo;
        }
    }

    foreach ($fileInfo->classInfos as $classInfo) {
        $classMap[$classInfo->name->__toString()] = $classInfo;
    }
}

if ($verify) {
    $errors = [];

    foreach ($funcMap as $aliasFunc) {
        if (!$aliasFunc->alias) {
            continue;
        }

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

        $aliasedReturn = $aliasedFunc->return;
        $aliasReturn = $aliasFunc->return;

        if (!$aliasedFunc->name->isConstructor() && !$aliasFunc->name->isConstructor()) {
            $aliasedReturnType = $aliasedReturn->type ?? $aliasedReturn->phpDocType;
            $aliasReturnType = $aliasReturn->type ?? $aliasReturn->phpDocType;
            if ($aliasReturnType != $aliasedReturnType) {
                $errors[] = "{$aliasFunc->name}() and {$aliasedFunc->name}() must have the same return type";
            }
        }

        $aliasedPhpDocReturnType = $aliasedReturn->phpDocType;
        $aliasPhpDocReturnType = $aliasReturn->phpDocType;
        if ($aliasedPhpDocReturnType != $aliasPhpDocReturnType && $aliasedPhpDocReturnType != $aliasReturn->type && $aliasPhpDocReturnType != $aliasedReturn->type) {
            $errors[] = "{$aliasFunc->name}() and {$aliasedFunc->name}() must have the same PHPDoc return type";
        }
    }

    echo implode("\n", $errors);
    if (!empty($errors)) {
        echo "\n";
        exit(1);
    }
}

if ($generateClassSynopses) {
    $classSynopsesDirectory = getcwd() . "/classsynopses";

    $classSynopses = generateClassSynopses($classMap);
    if (!empty($classSynopses)) {
        if (!file_exists($classSynopsesDirectory)) {
            mkdir($classSynopsesDirectory);
        }

        foreach ($classSynopses as $filename => $content) {
            if (file_put_contents("$classSynopsesDirectory/$filename", $content)) {
                echo "Saved $filename\n";
            }
        }
    }
}

if ($replaceClassSynopses) {
    $classSynopses = replaceClassSynopses($targetSynopses, $classMap);

    foreach ($classSynopses as $filename => $content) {
        if (file_put_contents($filename, $content)) {
            echo "Saved $filename\n";
        }
    }
}

if ($generateMethodSynopses) {
    $methodSynopsesDirectory = getcwd() . "/methodsynopses";

    $methodSynopses = generateMethodSynopses($funcMap, $aliasMap);
    if (!empty($methodSynopses)) {
        if (!file_exists($methodSynopsesDirectory)) {
            mkdir($methodSynopsesDirectory);
        }

        foreach ($methodSynopses as $filename => $content) {
            if (file_put_contents("$methodSynopsesDirectory/$filename", $content)) {
                echo "Saved $filename\n";
            }
        }
    }
}

if ($replaceMethodSynopses) {
    $methodSynopses = replaceMethodSynopses($targetSynopses, $funcMap, $aliasMap);

    foreach ($methodSynopses as $filename => $content) {
        if (file_put_contents($filename, $content)) {
            echo "Saved $filename\n";
        }
    }
}

if ($generateOptimizerInfo) {
    $filename = dirname(__FILE__, 2) . "/Zend/Optimizer/zend_func_infos.h";
    $optimizerInfo = generateOptimizerInfo($funcMap);

    if (file_put_contents($filename, $optimizerInfo)) {
        echo "Saved $filename\n";
    }
}
