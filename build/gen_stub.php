#!/usr/bin/env php
<?php declare(strict_types=1);

use PhpParser\Comment\Doc as DocComment;
use PhpParser\ConstExprEvaluator;
use PhpParser\Modifiers;
use PhpParser\Node;
use PhpParser\Node\AttributeGroup;
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
ini_set("precision", "-1");

const PHP_70_VERSION_ID = 70000;
const PHP_80_VERSION_ID = 80000;
const PHP_81_VERSION_ID = 80100;
const PHP_82_VERSION_ID = 80200;
const PHP_83_VERSION_ID = 80300;
const PHP_84_VERSION_ID = 80400;
const PHP_85_VERSION_ID = 80500;
const ALL_PHP_VERSION_IDS = [
    PHP_70_VERSION_ID,
    PHP_80_VERSION_ID,
    PHP_81_VERSION_ID,
    PHP_82_VERSION_ID,
    PHP_83_VERSION_ID,
    PHP_84_VERSION_ID,
    PHP_85_VERSION_ID,
];

// file_put_contents() but with a success message printed after saving
function reportFilePutContents(string $filename, string $content): void {
    if (file_put_contents($filename, $content)) {
        echo "Saved $filename\n";
    }
}

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
        if (substr($pathName, -9) === '.stub.php') {
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

function processStubFile(string $stubFile, Context $context, bool $includeOnly = false): ?FileInfo {
    try {
        if (!file_exists($stubFile)) {
            throw new Exception("File $stubFile does not exist");
        }

        if (!$includeOnly) {
            $stubFilenameWithoutExtension = str_replace(".stub.php", "", $stubFile);
            $arginfoFile = "{$stubFilenameWithoutExtension}_arginfo.h";
            $legacyFile = "{$stubFilenameWithoutExtension}_legacy_arginfo.h";

            $stubCode = file_get_contents($stubFile);
            $stubHash = sha1(str_replace("\r\n", "\n", $stubCode));
            $oldStubHash = extractStubHash($arginfoFile);
            if ($stubHash === $oldStubHash && !$context->forceParse) {
                /* Stub file did not change, do not regenerate. */
                return null;
            }
        }

        if (!$fileInfo = $context->parsedFiles[$stubFile] ?? null) {
            initPhpParser();
            $stubContent = $stubCode ?? file_get_contents($stubFile);
            $fileInfo = FileInfo::parseStubFile($stubContent);
            $context->parsedFiles[$stubFile] = $fileInfo;

            foreach ($fileInfo->dependencies as $dependency) {
                // TODO add header search path for extensions?
                $prefixes = [dirname($stubFile) . "/", dirname(__DIR__) . "/"];
                foreach ($prefixes as $prefix) {
                    $depFile = $prefix . $dependency;
                    if (file_exists($depFile)) {
                        break;
                    }
                    $depFile = null;
                }
                if (!$depFile) {
                    throw new Exception("File $stubFile includes a file $dependency which does not exist");
                }
                processStubFile($depFile, $context, true);
            }

            $constInfos = $fileInfo->getAllConstInfos();
            $context->allConstInfos = array_merge($context->allConstInfos, $constInfos);
        }

        if ($includeOnly) {
            return $fileInfo;
        }

        $arginfoCode = generateArgInfoCode(
            basename($stubFilenameWithoutExtension),
            $fileInfo,
            $context->allConstInfos,
            $stubHash
        );
        if ($context->forceRegeneration || $stubHash !== $oldStubHash) {
            reportFilePutContents($arginfoFile, $arginfoCode);
        }

        if ($fileInfo->shouldGenerateLegacyArginfo()) {
            $legacyFileInfo = $fileInfo->getLegacyVersion();

            $arginfoCode = generateArgInfoCode(
                basename($stubFilenameWithoutExtension),
                $legacyFileInfo,
                $context->allConstInfos,
                $stubHash
            );
            if ($context->forceRegeneration || $stubHash !== $oldStubHash) {
                reportFilePutContents($legacyFile, $arginfoCode);
            }
        }

        return $fileInfo;
    } catch (Exception $e) {
        echo "In $stubFile:\n{$e->getMessage()}\n";
        exit(1);
    }
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
    public bool $forceParse = false;
    public bool $forceRegeneration = false;
    /** @var array<string, ConstInfo> */
    public array $allConstInfos = [];
    /** @var FileInfo[] */
    public array $parsedFiles = [];
}

class ArrayType extends SimpleType {
    private /* readonly */ Type $keyType;
    private /* readonly */ Type $valueType;

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
    public /* readonly */ string $name;
    public /* readonly */ bool $isBuiltin;

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
            case "iterable":
                throw new Exception('This should not happen');
        }

        $matches = [];
        $isArray = preg_match("/(.*)\s*\[\s*\]/", $typeString, $matches);
        if ($isArray) {
            return new ArrayType(Type::fromString("int"), Type::fromString($matches[1]));
        }

        $matches = [];
        $isArray = preg_match("/array\s*<\s*([A-Za-z0-9_|-]+)?(\s*,\s*)?([A-Za-z0-9_|-]+)?\s*>/i", $typeString, $matches);
        if ($isArray) {
            if (empty($matches[1]) || empty($matches[3])) {
                throw new Exception("array<> type hint must have both a key and a value");
            }

            return new ArrayType(Type::fromString($matches[1]), Type::fromString($matches[3]));
        }

        return new SimpleType($typeString, false);
    }

    /**
     * @param mixed $value
     */
    public static function fromValue($value): SimpleType
    {
        switch (gettype($value)) {
            case "NULL":
                return SimpleType::null();
            case "boolean":
                return new SimpleType("bool", true);
            case "integer":
                return new SimpleType("int", true);
            case "double":
                return new SimpleType("float", true);
            case "string":
                return new SimpleType("string", true);
            case "array":
                return new SimpleType("array", true);
            case "object":
                return new SimpleType("object", true);
            default:
                throw new Exception("Type \"" . gettype($value) . "\" cannot be inferred based on value");
        }
    }

    public static function null(): SimpleType
    {
        return new SimpleType("null", true);
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

    public function isBool(): bool {
        return $this->isBuiltin && $this->name === 'bool';
    }

    public function isInt(): bool {
        return $this->isBuiltin && $this->name === 'int';
    }

    public function isFloat(): bool {
        return $this->isBuiltin && $this->name === 'float';
    }

    public function isString(): bool {
        return $this->isBuiltin && $this->name === 'string';
    }

    public function isArray(): bool {
        return $this->isBuiltin && $this->name === 'array';
    }

    public function isMixed(): bool {
        return $this->isBuiltin && $this->name === 'mixed';
    }

    private function toTypeInfo(): array {
        assert($this->isBuiltin);

        switch ($this->name) {
            case "null":
                return ["IS_NULL", "MAY_BE_NULL"];
            case "false":
                return ["IS_FALSE", "MAY_BE_FALSE"];
            case "true":
                return ["IS_TRUE", "MAY_BE_TRUE"];
            case "bool":
                return ["_IS_BOOL", "MAY_BE_BOOL"];
            case "int":
                return ["IS_LONG", "MAY_BE_LONG"];
            case "float":
                return ["IS_DOUBLE", "MAY_BE_DOUBLE"];
            case "string":
                return ["IS_STRING", "MAY_BE_STRING"];
            case "array":
                return ["IS_ARRAY", "MAY_BE_ARRAY"];
            case "object":
                return ["IS_OBJECT", "MAY_BE_OBJECT"];
            case "callable":
                return ["IS_CALLABLE", "MAY_BE_CALLABLE"];
            case "mixed":
                return ["IS_MIXED", "MAY_BE_ANY"];
            case "void":
                return ["IS_VOID", "MAY_BE_VOID"];
            case "static":
                return ["IS_STATIC", "MAY_BE_STATIC"];
            case "never":
                return ["IS_NEVER", "MAY_BE_NEVER"];
            default:
                throw new Exception("Not implemented: $this->name");
        }
    }

    public function toTypeCode(): string {
        return $this->toTypeInfo()[0];
    }

    public function toTypeMask(): string {
        return $this->toTypeInfo()[1];
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
            case "true":
                return "MAY_BE_ARRAY_OF_TRUE";
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
        // Escape backslashes, and also encode \u, \U, and \N to avoid compilation errors in generated macros
        return str_replace(
            ['\\', '\\u', '\\U', '\\N'],
            ['\\\\', '\\\\165', '\\\\125', '\\\\116'],
            $this->name
        );
    }

    public function toVarEscapedName(): string {
        return str_replace('\\', '_', $this->name);
    }

    public function equals(SimpleType $other): bool {
        return $this->name === $other->name && $this->isBuiltin === $other->isBuiltin;
    }
}

// Instances of Type are immutable and do not need to be cloned
// when held by an object that is cloned
class Type {
    /** @var SimpleType[] */
    public /* readonly */ array $types;
    public /* readonly */ bool $isIntersection;

    public static function fromNode(Node $node): Type {
        if ($node instanceof Node\UnionType || $node instanceof Node\IntersectionType) {
            $nestedTypeObjects = array_map(['Type', 'fromNode'], $node->types);
            $types = [];
            foreach ($nestedTypeObjects as $typeObject) {
                array_push($types, ...$typeObject->types);
            }
            return new Type($types, ($node instanceof Node\IntersectionType));
        }

        if ($node instanceof Node\NullableType) {
            return new Type(
                [
                    ...Type::fromNode($node->type)->types,
                    SimpleType::null(),
                ],
                false
            );
        }

        if ($node instanceof Node\Identifier && $node->toLowerString() === "iterable") {
            return new Type(
                [
                    SimpleType::fromString("Traversable"),
                    ArrayType::createGenericArray(),
                ],
                false
            );
        }

        return new Type([SimpleType::fromNode($node)], false);
    }

    public static function fromString(string $typeString): self {
        $typeString .= "|";
        $simpleTypes = [];
        $simpleTypeOffset = 0;
        $inArray = false;
        $isIntersection = false;

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

            if ($char === "|" || $char === "&") {
                $isIntersection = ($char === "&");
                $simpleTypeName = trim(substr($typeString, $simpleTypeOffset, $i - $simpleTypeOffset));

                $simpleTypes[] = SimpleType::fromString($simpleTypeName);

                $simpleTypeOffset = $i + 1;
            }
        }

        return new Type($simpleTypes, $isIntersection);
    }

    /**
     * @param SimpleType[] $types
     */
    private function __construct(array $types, bool $isIntersection) {
        $this->types = $types;
        $this->isIntersection = $isIntersection;
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

    public function tryToSimpleType(): ?SimpleType {
        $nonNullTypes = array_filter(
            $this->types,
            function(SimpleType $type) {
                return !$type->isNull();
            }
        );
        /* type has only null */
        if (count($nonNullTypes) === 0) {
            return $this->types[0];
        }
        if (count($nonNullTypes) === 1) {
            return reset($nonNullTypes);
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
            // TODO Support for toOptimizerMask for intersection
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
            $typeSort = $this->isIntersection ? "intersection" : "union";
            $typeElement = $doc->createElement('type');
            $typeElement->setAttribute("class", $typeSort);

            foreach ($this->types as $type) {
                $unionTypeElement = $doc->createElement('type', $type->name);
                $typeElement->appendChild($unionTypeElement);
            }
        } else {
            $type = $this->types[0];
            $typeElement = $doc->createElement('type', $type->name);
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
        $char = $this->isIntersection ? '&' : '|';
        return implode($char, array_map(
            function ($type) { return $type->name; },
            $this->types)
        );
    }
}

class ArginfoType {
    /** @var SimpleType[] $classTypes */
    public /* readonly */ array $classTypes;
    /** @var SimpleType[] $builtinTypes */
    private /* readonly */ array $builtinTypes;

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
    public const SEND_BY_VAL = "0";
    public const SEND_BY_REF = "1";
    public const SEND_PREFER_REF = "ZEND_SEND_PREFER_REF";

    public /* readonly */ string $name;
    public /* readonly */ string $sendBy;
    public /* readonly */ bool $isVariadic;
    public ?Type $type;
    private /* readonly */ ?Type $phpDocType;
    public ?string $defaultValue;
    /** @var AttributeInfo[] */
    public array $attributes;

    /**
     * @param AttributeInfo[] $attributes
     */
    public function __construct(
        string $name,
        string $sendBy,
        bool $isVariadic,
        ?Type $type,
        ?Type $phpDocType,
        ?string $defaultValue,
        array $attributes
    ) {
        $this->name = $name;
        $this->sendBy = $sendBy;
        $this->isVariadic = $isVariadic;
        $this->type = $type;
        $this->phpDocType = $phpDocType;
        $this->defaultValue = $defaultValue;
        $this->attributes = $attributes;
    }

    public function equals(ArgInfo $other): bool {
        return $this->name === $other->name
            && $this->sendBy === $other->sendBy
            && $this->isVariadic === $other->isVariadic
            && Type::equals($this->type, $other->type)
            && $this->defaultValue === $other->defaultValue;
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

    private function getDefaultValueAsArginfoString(): string {
        if ($this->hasProperDefaultValue()) {
            return '"' . addslashes($this->defaultValue) . '"';
        }

        return "NULL";
    }

    public function getDefaultValueAsMethodSynopsisString(): ?string {
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

    public function toZendInfo(): string {
        $argKind = $this->isVariadic ? "ARG_VARIADIC" : "ARG";
        $argDefaultKind = $this->hasProperDefaultValue() ? "_WITH_DEFAULT_VALUE" : "";
        $argType = $this->type;
        if ($argType !== null) {
            if (null !== $simpleArgType = $argType->tryToSimpleType()) {
                if ($simpleArgType->isBuiltin) {
                    return sprintf(
                        "\tZEND_%s_TYPE_INFO%s(%s, %s, %s, %d%s)\n",
                        $argKind, $argDefaultKind, $this->sendBy, $this->name,
                        $simpleArgType->toTypeCode(), $argType->isNullable(),
                        $this->hasProperDefaultValue() ? ", " . $this->getDefaultValueAsArginfoString() : ""
                    );
                }
                return sprintf(
                    "\tZEND_%s_OBJ_INFO%s(%s, %s, %s, %d%s)\n",
                    $argKind, $argDefaultKind, $this->sendBy, $this->name,
                    $simpleArgType->toEscapedName(), $argType->isNullable(),
                    $this->hasProperDefaultValue() ? ", " . $this->getDefaultValueAsArginfoString() : ""
                );
            }
            $arginfoType = $argType->toArginfoType();
            if ($arginfoType->hasClassType()) {
                return sprintf(
                    "\tZEND_%s_OBJ_TYPE_MASK(%s, %s, %s, %s%s)\n",
                    $argKind, $this->sendBy, $this->name,
                    $arginfoType->toClassTypeString(), $arginfoType->toTypeMask(),
                    !$this->isVariadic ? ", " . $this->getDefaultValueAsArginfoString() : ""
                );
            }
            return sprintf(
                "\tZEND_%s_TYPE_MASK(%s, %s, %s, %s)\n",
                $argKind, $this->sendBy, $this->name,
                $arginfoType->toTypeMask(),
                $this->getDefaultValueAsArginfoString()
            );
        }
        return sprintf(
            "\tZEND_%s_INFO%s(%s, %s%s)\n",
            $argKind, $argDefaultKind, $this->sendBy, $this->name,
            $this->hasProperDefaultValue() ? ", " . $this->getDefaultValueAsArginfoString() : ""
        );
    }
}

interface VariableLikeName {
    public function __toString(): string;
    public function getDeclarationName(): string;
}

abstract class AbstractConstName implements VariableLikeName
{
    public function isUnknown(): bool
    {
        return strtolower($this->__toString()) === "unknown";
    }
}

class ConstName extends AbstractConstName {
    private /* readonly */ string $const;

    public function __construct(?Name $namespace, string $const)
    {
        if ($namespace && ($namespace = $namespace->slice(0, -1))) {
            $const = $namespace->toString() . '\\' . $const;
        }
        $this->const = $const;
    }

    public function isUnknown(): bool
    {
        $name = $this->__toString();
        if (($pos = strrpos($name, '\\')) !== false) {
            $name = substr($name, $pos + 1);
        }
        return strtolower($name) === "unknown";
    }

    public function __toString(): string
    {
        return $this->const;
    }

    public function getDeclarationName(): string
    {
        return $this->name->toString();
    }
}

class ClassConstName extends AbstractConstName {
    public /* readonly */ Name $class;
    private /* readonly */ string $const;

    public function __construct(Name $class, string $const)
    {
        $this->class = $class;
        $this->const = $const;
    }

    public function __toString(): string
    {
        return $this->class->toString() . "::" . $this->const;
    }

    public function getDeclarationName(): string
    {
        return $this->const;
    }
}

class PropertyName implements VariableLikeName {
    public /* readonly */ Name $class;
    private /* readonly */ string $property;

    public function __construct(Name $class, string $property)
    {
        $this->class = $class;
        $this->property = $property;
    }

    public function __toString(): string
    {
        return $this->class->toString() . "::$" . $this->property;
    }

    public function getDeclarationName(): string
    {
        return $this->property;
    }
}

interface FunctionOrMethodName {
    public function getDeclaration(): string;
    public function getArgInfoName(): string;
    public function getMethodSynopsisFilename(): string;
    public function getNameForAttributes(): string;
    public function __toString(): string;
    public function isConstructor(): bool;
    public function isDestructor(): bool;
}

class FunctionName implements FunctionOrMethodName {
    private /* readonly */ Name $name;

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
        return implode('_', $this->name->getParts());
    }

    public function getFunctionName(): string {
        return $this->name->getLast();
    }

    public function getDeclaration(): string {
        return "ZEND_FUNCTION({$this->getDeclarationName()});\n";
    }

    public function getArgInfoName(): string {
        $underscoreName = implode('_', $this->name->getParts());
        return "arginfo_$underscoreName";
    }

    public function getFramelessFunctionInfosName(): string {
        $underscoreName = implode('_', $this->name->getParts());
        return "frameless_function_infos_$underscoreName";
    }

    public function getMethodSynopsisFilename(): string {
        return 'functions/' . implode('/', str_replace('_', '-', $this->name->getParts()));
    }

    public function getNameForAttributes(): string {
        return strtolower($this->name->toString());
    }

    public function __toString(): string {
        return $this->name->toString();
    }

    public function isConstructor(): bool {
        return false;
    }

    public function isDestructor(): bool {
        return false;
    }
}

class MethodName implements FunctionOrMethodName {
    public /* readonly */ Name $className;
    public /* readonly */ string $methodName;

    public function __construct(Name $className, string $methodName) {
        $this->className = $className;
        $this->methodName = $methodName;
    }

    public function getDeclarationClassName(): string {
        return implode('_', $this->className->getParts());
    }

    public function getDeclaration(): string {
        return "ZEND_METHOD({$this->getDeclarationClassName()}, $this->methodName);\n";
    }

    public function getArgInfoName(): string {
        return "arginfo_class_{$this->getDeclarationClassName()}_{$this->methodName}";
    }

    public function getMethodSynopsisFilename(): string
    {
        $parts = [...$this->className->getParts(), ltrim($this->methodName, '_')];
        /* File paths are in lowercase */
        return strtolower(implode('/', $parts));
    }

    public function getNameForAttributes(): string {
        return strtolower($this->methodName);
    }

    public function __toString(): string {
        return "$this->className::$this->methodName";
    }

    public function isConstructor(): bool {
        return $this->methodName === "__construct";
    }

    public function isDestructor(): bool {
        return $this->methodName === "__destruct";
    }
}

class ReturnInfo {
    public const REFCOUNT_0 = "0";
    public const REFCOUNT_1 = "1";
    public const REFCOUNT_N = "N";

    public const REFCOUNTS_NONSCALAR = [
        self::REFCOUNT_1,
        self::REFCOUNT_N,
    ];

    private /* readonly */ bool $byRef;
    // NOT readonly - gets removed when discarding info for older PHP versions
    public ?Type $type;
    public /* readonly */ ?Type $phpDocType;
    public /* readonly */ bool $tentativeReturnType;
    public /* readonly */ string $refcount;

    public function __construct(bool $byRef, ?Type $type, ?Type $phpDocType, bool $tentativeReturnType, ?string $refcount) {
        $this->byRef = $byRef;
        $this->type = $type;
        $this->phpDocType = $phpDocType;
        $this->tentativeReturnType = $tentativeReturnType;
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

    private function setRefcount(?string $refcount): void
    {
        $type = $this->phpDocType ?? $this->type;
        $isScalarType = $type !== null && $type->isScalar();

        if ($refcount === null) {
            $this->refcount = $isScalarType ? self::REFCOUNT_0 : self::REFCOUNT_N;
            return;
        }

        if ($isScalarType) {
            throw new Exception(
                "@refcount on functions returning scalar values is redundant and not permitted"
            );
        }

        if (!in_array($refcount, ReturnInfo::REFCOUNTS_NONSCALAR, true)) {
            throw new Exception("@refcount must have one of the following values: \"1\", \"N\", $refcount given");
        }

        $this->refcount = $refcount;
    }

    public function beginArgInfo(string $funcInfoName, int $minArgs, bool $php81MinimumCompatibility): string {
        $code = $this->beginArgInfoCompatible($funcInfoName, $minArgs);
        if ($this->type !== null && $this->tentativeReturnType && !$php81MinimumCompatibility) {
            $realCode = "#if (PHP_VERSION_ID >= " . PHP_81_VERSION_ID . ")\n";
            $realCode .= $code;
            $realCode .= sprintf(
                "#else\nZEND_BEGIN_ARG_INFO_EX(%s, 0, %d, %d)\n#endif\n",
                $funcInfoName, $this->byRef, $minArgs
            );
            return $realCode;
        }
        return $code;
    }

    /**
     * Assumes PHP 8.1 compatibility, if that is not the case the caller is
     * responsible for making the use of a tentative return type conditional
     * based on the PHP version. Separate to allow using early returns
     */
    private function beginArgInfoCompatible(string $funcInfoName, int $minArgs): string {
        if ($this->type !== null) {
            if (null !== $simpleReturnType = $this->type->tryToSimpleType()) {
                if ($simpleReturnType->isBuiltin) {
                    return sprintf(
                        "%s(%s, %d, %d, %s, %d)\n",
                        $this->tentativeReturnType ? "ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX" : "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX",
                        $funcInfoName, $this->byRef,
                        $minArgs,
                        $simpleReturnType->toTypeCode(), $this->type->isNullable()
                    );
                }
                return sprintf(
                    "%s(%s, %d, %d, %s, %d)\n",
                    $this->tentativeReturnType ? "ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX" : "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX",
                    $funcInfoName, $this->byRef,
                    $minArgs,
                    $simpleReturnType->toEscapedName(), $this->type->isNullable()
                );
            }
            $arginfoType = $this->type->toArginfoType();
            if ($arginfoType->hasClassType()) {
                return sprintf(
                    "%s(%s, %d, %d, %s, %s)\n",
                    $this->tentativeReturnType ? "ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX" : "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX",
                    $funcInfoName, $this->byRef,
                    $minArgs,
                    $arginfoType->toClassTypeString(), $arginfoType->toTypeMask()
                );
            }
            return sprintf(
                "%s(%s, %d, %d, %s)\n",
                $this->tentativeReturnType ? "ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX" : "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX",
                $funcInfoName, $this->byRef,
                $minArgs,
                $arginfoType->toTypeMask()
            );
        }
        return sprintf(
            "ZEND_BEGIN_ARG_INFO_EX(%s, 0, %d, %d)\n",
            $funcInfoName, $this->byRef, $minArgs
        );
    }
}

class VersionFlags {

    /**
     * Keys are the PHP versions, values are arrays of flags
     */
    private array $flagsByVersion;

    public function __construct(array $baseFlags) {
        $this->flagsByVersion = [];
        foreach (ALL_PHP_VERSION_IDS as $version) {
            $this->flagsByVersion[$version] = $baseFlags;
        }
    }

    public function addForVersionsAbove(string $flag, int $minimumVersionId): void {
        $write = false;

        foreach (ALL_PHP_VERSION_IDS as $version) {
            if ($version === $minimumVersionId || $write === true) {
                $this->flagsByVersion[$version][] = $flag;
                $write = true;
            }
        }
    }

    public function isEmpty(): bool {
        foreach (ALL_PHP_VERSION_IDS as $version) {
            if ($this->flagsByVersion[$version] !== []) {
                return false;
            }
        }
        return true;
    }

    public function generateVersionDependentFlagCode(
        string $codeTemplate,
        ?int $phpVersionIdMinimumCompatibility,
        ?int $phpVersionIdMaxCompatibility = null
    ): string {
        $flagsByPhpVersions = $this->flagsByVersion;
        $phpVersions = ALL_PHP_VERSION_IDS;
        sort($phpVersions);
        $currentPhpVersion = end($phpVersions);

        // No version compatibility is needed
        if ($phpVersionIdMinimumCompatibility === null) {
            if (empty($flagsByPhpVersions[$currentPhpVersion])) {
                return '';
            }
            return sprintf($codeTemplate, implode("|", $flagsByPhpVersions[$currentPhpVersion]));
        }

        ksort($flagsByPhpVersions);
        // Remove flags which depend on a PHP version below the minimally supported one
        $index = array_search($phpVersionIdMinimumCompatibility, array_keys($flagsByPhpVersions));
        if ($index === false) {
            throw new Exception("Missing version dependent flags for PHP version ID \"$phpVersionIdMinimumCompatibility\"");
        }
        $flagsByPhpVersions = array_slice($flagsByPhpVersions, $index, null, true);
        if ($phpVersionIdMaxCompatibility !== null) {
            // Remove flags which depend on a PHP version above the maximally supported one
            $index = array_search($phpVersionIdMaxCompatibility, array_keys($flagsByPhpVersions));
            if ($index === false) {
                throw new Exception("Missing version dependent flags for PHP version ID \"$phpVersionIdMaxCompatibility\"");
            }
            $flagsByPhpVersions = array_slice($flagsByPhpVersions, 0, $index, true);
        }

        // Remove version-specific flags which don't differ from the previous one
        // Also populate '0' values
        $previousVersionId = null;
        foreach ($flagsByPhpVersions as $versionId => $versionFlags) {
            if ($versionFlags === []) {
                $versionFlags = ['0'];
                $flagsByPhpVersions[$versionId] = ['0'];
            }
            if ($previousVersionId !== null && $flagsByPhpVersions[$previousVersionId] === $versionFlags) {
                unset($flagsByPhpVersions[$versionId]);
            } else {
                $previousVersionId = $versionId;
            }
        }

        $flagCount = count($flagsByPhpVersions);

        // Do not add a condition unnecessarily when the only version is the same as the minimally supported one
        if ($flagCount === 1) {
            reset($flagsByPhpVersions);
            $firstVersion = key($flagsByPhpVersions);
            if ($firstVersion === $phpVersionIdMinimumCompatibility) {
                return sprintf($codeTemplate, implode("|", reset($flagsByPhpVersions)));
            }
        }

        // Add the necessary conditions around the code using the version-specific flags
        $code = '';
        $i = 0;
        foreach (array_reverse($flagsByPhpVersions, true) as $version => $versionFlags) {
            $if = $i === 0 ? "#if" : "#elif";
            $endif = $i === $flagCount - 1 ? "#endif\n" : "";

            $code .= "$if (PHP_VERSION_ID >= $version)\n";

            $code .= sprintf($codeTemplate, implode("|", $versionFlags));
            $code .= $endif;

            $i++;
        }

        return $code;
    }
}

class FuncInfo {
    public /* readonly */ FunctionOrMethodName $name;
    private /* readonly */ int $classFlags;
    public int $flags;
    public /* readonly */ ?string $aliasType;
    public ?FunctionOrMethodName $alias;
    private /* readonly */ bool $isDeprecated;
    private bool $supportsCompileTimeEval;
    public /* readonly */ bool $verify;
    /** @var ArgInfo[] */
    public /* readonly */ array $args;
    public /* readonly */ ReturnInfo $return;
    private /* readonly */ int $numRequiredArgs;
    public /* readonly */ ?string $cond;
    public bool $isUndocumentable;
    private ?int $minimumPhpVersionIdCompatibility;
    /** @var AttributeInfo[] */
    public array $attributes;
    /** @var FramelessFunctionInfo[] */
    private array $framelessFunctionInfos;
    private ?ExposedDocComment $exposedDocComment;

    /**
     * @param ArgInfo[] $args
     * @param AttributeInfo[] $attributes
     * @param FramelessFunctionInfo[] $framelessFunctionInfos
     */
    public function __construct(
        FunctionOrMethodName $name,
        int $classFlags,
        int $flags,
        ?string $aliasType,
        ?FunctionOrMethodName $alias,
        bool $isDeprecated,
        bool $supportsCompileTimeEval,
        bool $verify,
        array $args,
        ReturnInfo $return,
        int $numRequiredArgs,
        ?string $cond,
        bool $isUndocumentable,
        ?int $minimumPhpVersionIdCompatibility,
        array $attributes,
        array $framelessFunctionInfos,
        ?ExposedDocComment $exposedDocComment
    ) {
        $this->name = $name;
        $this->classFlags = $classFlags;
        $this->flags = $flags;
        $this->aliasType = $aliasType;
        $this->alias = $alias;
        $this->isDeprecated = $isDeprecated;
        $this->supportsCompileTimeEval = $supportsCompileTimeEval;
        $this->verify = $verify;
        $this->args = $args;
        $this->return = $return;
        $this->numRequiredArgs = $numRequiredArgs;
        $this->cond = $cond;
        $this->isUndocumentable = $isUndocumentable;
        $this->minimumPhpVersionIdCompatibility = $minimumPhpVersionIdCompatibility;
        $this->attributes = $attributes;
        $this->framelessFunctionInfos = $framelessFunctionInfos;
        $this->exposedDocComment = $exposedDocComment;
        if ($return->tentativeReturnType && $this->isFinalMethod()) {
            throw new Exception("Tentative return inapplicable for final method");
        }
    }

    public function isMethod(): bool
    {
        return $this->name instanceof MethodName;
    }

    private function isFinalMethod(): bool
    {
        return ($this->flags & Modifiers::FINAL) || ($this->classFlags & Modifiers::FINAL);
    }

    public function isInstanceMethod(): bool
    {
        return !($this->flags & Modifiers::STATIC) && $this->isMethod() && !$this->name->isConstructor();
    }

    /** @return string[] */
    private function getModifierNames(): array
    {
        if (!$this->isMethod()) {
            return [];
        }

        $result = [];

        if ($this->flags & Modifiers::FINAL) {
            $result[] = "final";
        } elseif ($this->flags & Modifiers::ABSTRACT && $this->classFlags & ~Modifiers::ABSTRACT) {
            $result[] = "abstract";
        }

        if ($this->flags & Modifiers::PROTECTED) {
            $result[] = "protected";
        } elseif ($this->flags & Modifiers::PRIVATE) {
            $result[] = "private";
        } else {
            $result[] = "public";
        }

        if ($this->flags & Modifiers::STATIC) {
            $result[] = "static";
        }

        return $result;
    }

    private function hasParamWithUnknownDefaultValue(): bool
    {
        foreach ($this->args as $arg) {
            if ($arg->defaultValue && !$arg->hasProperDefaultValue()) {
                return true;
            }
        }

        return false;
    }

    private function equalsApartFromNameAndRefcount(FuncInfo $other): bool {
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
        if ($this->flags & Modifiers::ABSTRACT) {
            return null;
        }

        $name = $this->alias ?? $this->name;

        return $name->getDeclaration();
    }

    public function getFramelessDeclaration(): ?string {
        if (empty($this->framelessFunctionInfos)) {
            return null;
        }

        $code = '';
        $infos = '';
        foreach ($this->framelessFunctionInfos as $framelessFunctionInfo) {
            $code .= "ZEND_FRAMELESS_FUNCTION({$this->name->getFunctionName()}, {$framelessFunctionInfo->arity});\n";
            $infos .= "\t{ ZEND_FRAMELESS_FUNCTION_NAME({$this->name->getFunctionName()}, {$framelessFunctionInfo->arity}), {$framelessFunctionInfo->arity} },\n";
        }

        $code .= 'static const zend_frameless_function_info ' . $this->getFramelessFunctionInfosName() . "[] = {\n";
        $code .= $infos;
        $code .= "\t{ 0 },\n";
        $code .= "};\n";

        $php84MinimumCompatibility = $this->minimumPhpVersionIdCompatibility === null || $this->minimumPhpVersionIdCompatibility >= PHP_84_VERSION_ID;
        if (!$php84MinimumCompatibility) {
            return "#if (PHP_VERSION_ID >= " . PHP_84_VERSION_ID . ")\n$code#endif\n";
        }

        return $code;
    }

    private function getFramelessFunctionInfosName(): string {
        return $this->name->getFramelessFunctionInfosName();
    }

    public function getFunctionEntry(): string {
        if (!empty($this->framelessFunctionInfos)) {
            if ($this->isMethod()) {
                throw new Exception('Frameless methods are not supported yet');
            }
            if ($this->name->getNamespace()) {
                throw new Exception('Namespaced direct calls to frameless functions are not supported yet');
            }
            if ($this->alias) {
                throw new Exception('Aliased direct calls to frameless functions are not supported yet');
            }
        }

        $isVanillaEntry = $this->alias === null && !$this->supportsCompileTimeEval && $this->exposedDocComment === null && empty($this->framelessFunctionInfos);
        $argInfoName = $this->getArgInfoName();
        $flagsByPhpVersions = $this->getArginfoFlagsByPhpVersions();

        if ($this->isMethod()) {
            $zendName = '"' . $this->name->methodName . '"';
            if ($this->alias) {
                if ($this->alias instanceof MethodName) {
                    $name = "zim_" . $this->alias->getDeclarationClassName() . "_" . $this->alias->methodName;
                } else if ($this->alias instanceof FunctionName) {
                    $name = "zif_" . $this->alias->getNonNamespacedName();
                } else {
                    throw new Error("Cannot happen");
                }
            } elseif ($this->flags & Modifiers::ABSTRACT) {
                $name = "NULL";
            } else {
                $name = "zim_" . $this->name->getDeclarationClassName() . "_" . $this->name->methodName;

                if ($isVanillaEntry) {
                    $template = "\tZEND_ME(" . $this->name->getDeclarationClassName() . ", " . $this->name->methodName . ", $argInfoName, %s)\n";
                    $flagsCode = $flagsByPhpVersions->generateVersionDependentFlagCode(
                        $template,
                        $this->minimumPhpVersionIdCompatibility
                    );
                    return rtrim($flagsCode) . "\n";
                }
            }
        } else if ($this->name instanceof FunctionName) {
            $functionName = $this->name->getFunctionName();
            $declarationName = $this->alias ? $this->alias->getNonNamespacedName() : $this->name->getDeclarationName();
            $name = "zif_$declarationName";

            if ($this->name->getNamespace()) {
                $namespace = addslashes($this->name->getNamespace());
                $zendName = "ZEND_NS_NAME(\"$namespace\", \"$functionName\")";
            } else {
                // Can only use ZEND_FE() if we have no flags for *all* versions
                if ($isVanillaEntry && $flagsByPhpVersions->isEmpty()) {
                    return "\tZEND_FE($declarationName, $argInfoName)\n";
                }
                $zendName = '"' . $functionName . '"';
            }
        } else {
            throw new Error("Cannot happen");
        }

        $docComment = $this->exposedDocComment ? '"' . $this->exposedDocComment->escape() . '"' : "NULL";
        $framelessFuncInfosName = !empty($this->framelessFunctionInfos) ? $this->getFramelessFunctionInfosName() : "NULL";

        // Assume 8.4+ here, if older versions are supported this is conditional
        $code = $flagsByPhpVersions->generateVersionDependentFlagCode(
            "\tZEND_RAW_FENTRY($zendName, $name, $argInfoName, %s, $framelessFuncInfosName, $docComment)\n",
            PHP_84_VERSION_ID
        );

        $php84MinimumCompatibility = $this->minimumPhpVersionIdCompatibility === null || $this->minimumPhpVersionIdCompatibility >= PHP_84_VERSION_ID;
        if (!$php84MinimumCompatibility) {
            $code = "#if (PHP_VERSION_ID >= " . PHP_84_VERSION_ID . ")\n$code";
            $code .= "#else\n";

            $code .= $flagsByPhpVersions->generateVersionDependentFlagCode(
                "\tZEND_RAW_FENTRY($zendName, $name, $argInfoName, %s)\n",
                $this->minimumPhpVersionIdCompatibility,
                PHP_83_VERSION_ID
            );

            $code .= "#endif\n";
        }

        return $code;
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

        return "\tF" . $this->return->refcount . '("' . addslashes($this->name->__toString()) . '", ' . $type->toOptimizerTypeMask() . "),\n";
    }

    public function discardInfoForOldPhpVersions(?int $minimumPhpVersionIdCompatibility): void {
        $this->attributes = [];
        $this->return->type = null;
        $this->framelessFunctionInfos = [];
        $this->exposedDocComment = null;
        $this->supportsCompileTimeEval = false;
        foreach ($this->args as $arg) {
            $arg->type = null;
            $arg->defaultValue = null;
            $arg->attributes = [];
        }
        $this->minimumPhpVersionIdCompatibility = $minimumPhpVersionIdCompatibility;
    }

    private function getArginfoFlagsByPhpVersions(): VersionFlags
    {
        $flags = [];

        if ($this->isMethod()) {
            if ($this->flags & Modifiers::PROTECTED) {
                $flags[] = "ZEND_ACC_PROTECTED";
            } elseif ($this->flags & Modifiers::PRIVATE) {
                $flags[] = "ZEND_ACC_PRIVATE";
            } else {
                $flags[] = "ZEND_ACC_PUBLIC";
            }

            if ($this->flags & Modifiers::STATIC) {
                $flags[] = "ZEND_ACC_STATIC";
            }

            if ($this->flags & Modifiers::FINAL) {
                $flags[] = "ZEND_ACC_FINAL";
            }

            if ($this->flags & Modifiers::ABSTRACT) {
                $flags[] = "ZEND_ACC_ABSTRACT";
            }
        }

        if ($this->isDeprecated) {
            $flags[] = "ZEND_ACC_DEPRECATED";
        }

        foreach ($this->attributes as $attr) {
            switch ($attr->class) {
                case "Deprecated":
                    $flags[] = "ZEND_ACC_DEPRECATED";
                    break;
            }
        }

        $flags = new VersionFlags($flags);

        if ($this->isMethod() === false && $this->supportsCompileTimeEval) {
            $flags->addForVersionsAbove("ZEND_ACC_COMPILE_TIME_EVAL", PHP_82_VERSION_ID);
        }

        foreach ($this->attributes as $attr) {
            switch ($attr->class) {
                case "NoDiscard":
                    $flags->addForVersionsAbove("ZEND_ACC_NODISCARD", PHP_85_VERSION_ID);
                    break;
            }
        }

        return $flags;
    }

    private function generateRefSect1(DOMDocument $doc, string $role): DOMElement {
        $refSec = $doc->createElement('refsect1');
        $refSec->setAttribute('role', $role);
        $refSec->append(
            "\n  ",
            $doc->createEntityReference('reftitle.' . $role),
            "\n  "
        );
        return $refSec;
    }

    public function getMethodSynopsisDocument(): ?string {
        $REFSEC1_SEPERATOR = "\n\n ";

        $doc = new DOMDocument("1.0", "utf-8");
        $doc->formatOutput = true;

        $refentry = $doc->createElement('refentry');
        $doc->appendChild($refentry);

        if ($this->isMethod()) {
            assert($this->name instanceof MethodName);
            /* Namespaces are seperated by '-', '_' must be converted to '-' too.
             * Trim away the __ for magic methods */
            $id = strtolower(
                str_replace('\\', '-', $this->name->className->__toString())
                . '.'
                . str_replace('_', '-', ltrim($this->name->methodName, '_'))
            );
        } else {
            $id = 'function.' . strtolower(str_replace('_', '-', $this->name->__toString()));
        }
        $refentry->setAttribute("xml:id", $id);
        /* We create an attribute for xmlns, as libxml otherwise force it to be the first one */
        //$refentry->setAttribute("xmlns", "http://docbook.org/ns/docbook");
        $namespace = $doc->createAttribute('xmlns');
        $namespace->value = "http://docbook.org/ns/docbook";
        $refentry->setAttributeNode($namespace);
        $refentry->setAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
        $refentry->appendChild(new DOMText("\n "));

        /* Creation of <refnamediv> */
        $refnamediv = $doc->createElement('refnamediv');
        $refnamediv->appendChild(new DOMText("\n  "));
        $refname = $doc->createElement('refname', $this->name->__toString());
        $refnamediv->appendChild($refname);
        $refnamediv->appendChild(new DOMText("\n  "));
        $refpurpose = $doc->createElement('refpurpose', 'Description');
        $refnamediv->appendChild($refpurpose);

        $refnamediv->appendChild(new DOMText("\n "));
        $refentry->append($refnamediv, $REFSEC1_SEPERATOR);

        /* Creation of <refsect1 role="description"> */
        $descriptionRefSec = $this->generateRefSect1($doc, 'description');

        $methodSynopsis = $this->getMethodSynopsisElement($doc);
        if (!$methodSynopsis) {
            return null;
        }
        $descriptionRefSec->appendChild($methodSynopsis);
        $descriptionRefSec->appendChild(new DOMText("\n  "));
        $undocumentedEntity = $doc->createEntityReference('warn.undocumented.func');
        $descriptionRefSec->appendChild($undocumentedEntity);
        $descriptionRefSec->appendChild(new DOMText("\n  "));
        $returnDescriptionPara = $doc->createElement('simpara');
        $returnDescriptionPara->appendChild(new DOMText("\n   Description.\n  "));
        $descriptionRefSec->appendChild($returnDescriptionPara);

        $descriptionRefSec->appendChild(new DOMText("\n "));
        $refentry->append($descriptionRefSec, $REFSEC1_SEPERATOR);

        /* Creation of <refsect1 role="parameters"> */
        $parametersRefSec = $this->getParameterSection($doc);
        $refentry->append($parametersRefSec, $REFSEC1_SEPERATOR);

        /* Creation of <refsect1 role="returnvalues"> */
        if (!$this->name->isConstructor() && !$this->name->isDestructor()) {
            $returnRefSec = $this->getReturnValueSection($doc);
            $refentry->append($returnRefSec, $REFSEC1_SEPERATOR);
        }

        /* Creation of <refsect1 role="errors"> */
        $errorsRefSec = $this->generateRefSect1($doc, 'errors');
        $errorsDescriptionParaConstantTag = $doc->createElement('constant');
        $errorsDescriptionParaConstantTag->append('E_*');
        $errorsDescriptionParaExceptionTag = $doc->createElement('exceptionname');
        $errorsDescriptionParaExceptionTag->append('Exception');
        $errorsDescriptionPara = $doc->createElement('simpara');
        $errorsDescriptionPara->append(
            "\n   When does this function issue ",
            $errorsDescriptionParaConstantTag,
            " level errors,\n   and/or throw ",
            $errorsDescriptionParaExceptionTag,
            "s.\n  "
        );
        $errorsRefSec->appendChild($errorsDescriptionPara);
        $errorsRefSec->appendChild(new DOMText("\n "));

        $refentry->append($errorsRefSec, $REFSEC1_SEPERATOR);

        /* Creation of <refsect1 role="changelog"> */
        $changelogRefSec = $this->getChangelogSection($doc);
        $refentry->append($changelogRefSec, $REFSEC1_SEPERATOR);

        $exampleRefSec = $this->getExampleSection($doc, $id);
        $refentry->append($exampleRefSec, $REFSEC1_SEPERATOR);

        /* Creation of <refsect1 role="notes"> */
        $notesRefSec = $this->generateRefSect1($doc, 'notes');

        $noteTagSimara = $doc->createElement('simpara');
        $noteTagSimara->append(
            "\n    Any notes that don't fit anywhere else should go here.\n   "
        );
        $noteTag = $doc->createElement('note');
        $noteTag->append("\n   ", $noteTagSimara, "\n  ");
        $notesRefSec->append($noteTag, "\n ");

        $refentry->append($notesRefSec, $REFSEC1_SEPERATOR);

        /* Creation of <refsect1 role="seealso"> */
        $seeAlsoRefSec = $this->generateRefSect1($doc, 'seealso');

        $seeAlsoMemberClassMethod = $doc->createElement('member');
        $seeAlsoMemberClassMethodTag = $doc->createElement('methodname');
        $seeAlsoMemberClassMethodTag->appendChild(new DOMText("ClassName::otherMethodName"));
        $seeAlsoMemberClassMethod->appendChild($seeAlsoMemberClassMethodTag);

        $seeAlsoMemberFunction = $doc->createElement('member');
        $seeAlsoMemberFunctionTag = $doc->createElement('function');
        $seeAlsoMemberFunctionTag->appendChild(new DOMText("some_function"));
        $seeAlsoMemberFunction->appendChild($seeAlsoMemberFunctionTag);

        $seeAlsoMemberLink = $doc->createElement('member');
        $seeAlsoMemberLinkTag = $doc->createElement('link');
        $seeAlsoMemberLinkTag->setAttribute('linkend', 'some.id.chunk.to.link');
        $seeAlsoMemberLinkTag->appendChild(new DOMText('something appendix'));
        $seeAlsoMemberLink->appendChild($seeAlsoMemberLinkTag);

        $seeAlsoList = $doc->createElement('simplelist');
        $seeAlsoList->append(
            "\n   ",
            $seeAlsoMemberClassMethod,
            "\n   ",
            $seeAlsoMemberFunction,
            "\n   ",
            $seeAlsoMemberLink,
            "\n  "
        );

        $seeAlsoRefSec->appendChild($seeAlsoList);
        $seeAlsoRefSec->appendChild(new DOMText("\n "));

        $refentry->appendChild($seeAlsoRefSec);

        $refentry->appendChild(new DOMText("\n\n"));

        $doc->appendChild(new DOMComment(
            <<<ENDCOMMENT
 Keep this comment at the end of the file
Local variables:
mode: sgml
sgml-omittag:t
sgml-shorttag:t
sgml-minimize-attributes:nil
sgml-always-quote-attributes:t
sgml-indent-step:1
sgml-indent-data:t
indent-tabs-mode:nil
sgml-parent-document:nil
sgml-default-dtd-file:"~/.phpdoc/manual.ced"
sgml-exposed-tags:nil
sgml-local-catalogs:nil
sgml-local-ecat-files:nil
End:
vim600: syn=xml fen fdm=syntax fdl=2 si
vim: et tw=78 syn=sgml
vi: ts=1 sw=1

ENDCOMMENT
        ));
        return $doc->saveXML();
    }

    private function getParameterSection(DOMDocument $doc): DOMElement {
        $parametersRefSec = $this->generateRefSect1($doc, 'parameters');
        if (empty($this->args)) {
            $noParamEntity = $doc->createEntityReference('no.function.parameters');
            $parametersRefSec->appendChild($noParamEntity);
            return $parametersRefSec;
        } else {
            $parametersContainer = $doc->createDocumentFragment();

            $parametersContainer->appendChild(new DOMText("\n  "));
            $parametersList = $doc->createElement('variablelist');
            $parametersContainer->appendChild($parametersList);

            /*
            <varlistentry>
             <term><parameter>name</parameter></term>
             <listitem>
              <simpara>
               Description.
              </simpara>
             </listitem>
            </varlistentry>
            */
            foreach ($this->args as $arg) {
                $parameter = $doc->createElement('parameter', $arg->name);
                $parameterTerm = $doc->createElement('term');
                $parameterTerm->appendChild($parameter);

                $listItemPara = $doc->createElement('simpara');
                $listItemPara->append(
                    "\n      ",
                    "Description.",
                    "\n     ",
                );

                $parameterEntryListItem = $doc->createElement('listitem');
                $parameterEntryListItem->append(
                    "\n     ",
                    $listItemPara,
                    "\n    ",
                );

                $parameterEntry = $doc->createElement('varlistentry');
                $parameterEntry->append(
                    "\n    ",
                    $parameterTerm,
                    "\n    ",
                    $parameterEntryListItem,
                    "\n   ",
                );

                $parametersList->appendChild(new DOMText("\n   "));
                $parametersList->appendChild($parameterEntry);
            }
            $parametersList->appendChild(new DOMText("\n  "));
        }
        $parametersContainer->appendChild(new DOMText("\n "));
        $parametersRefSec->appendChild($parametersContainer);
        $parametersRefSec->appendChild(new DOMText("\n "));
        return $parametersRefSec;
    }

    private function getReturnValueSection(DOMDocument $doc): DOMElement {
        $returnRefSec = $this->generateRefSect1($doc, 'returnvalues');

        $returnDescriptionPara = $doc->createElement('simpara');
        $returnDescriptionPara->appendChild(new DOMText("\n   "));

        $returnType = $this->return->getMethodSynopsisType();
        if ($returnType === null) {
            $returnDescriptionPara->appendChild(new DOMText("Description."));
        } else if (count($returnType->types) === 1) {
            $type = $returnType->types[0];

            switch ($type->name) {
                case 'void':
                    $descriptionNode = $doc->createEntityReference('return.void');
                    break;
                case 'true':
                    $descriptionNode = $doc->createEntityReference('return.true.always');
                    break;
                case 'bool':
                    $descriptionNode = $doc->createEntityReference('return.success');
                    break;
                default:
                    $descriptionNode = new DOMText("Description.");
                    break;
            }
            $returnDescriptionPara->appendChild($descriptionNode);
        } else {
            $returnDescriptionPara->appendChild(new DOMText("Description."));
        }
        $returnDescriptionPara->appendChild(new DOMText("\n  "));
        $returnRefSec->appendChild($returnDescriptionPara);
        $returnRefSec->appendChild(new DOMText("\n "));
        return $returnRefSec;
    }

    /**
     * @param array<DOMNode> $headers [count($headers) === $columns]
     * @param array<array<DOMNode>> $rows [count($rows[$i]) === $columns]
     */
    private function generateDocbookInformalTable(
        DOMDocument $doc,
        int $indent,
        int $columns,
        array $headers,
        array $rows
    ): DOMElement {
        $strIndent = str_repeat(' ', $indent);

        $headerRow = $doc->createElement('row');
        foreach ($headers as $header) {
            $headerEntry = $doc->createElement('entry');
            $headerEntry->appendChild($header);

            $headerRow->append("\n$strIndent    ", $headerEntry);
        }
        $headerRow->append("\n$strIndent   ");

        $thead = $doc->createElement('thead');
        $thead->append(
            "\n$strIndent   ",
            $headerRow,
            "\n$strIndent  ",
        );

        $tbody = $doc->createElement('tbody');
        foreach ($rows as $row) {
            $bodyRow = $doc->createElement('row');
            foreach ($row as $cell) {
                $entry = $doc->createElement('entry');
                $entry->appendChild($cell);

                $bodyRow->appendChild(new DOMText("\n$strIndent    "));
                $bodyRow->appendChild($entry);
            }
            $bodyRow->appendChild(new DOMText("\n$strIndent   "));

            $tbody->append(
                "\n$strIndent   ",
                $bodyRow,
                "\n$strIndent  ",
            );
        }

        $tgroup = $doc->createElement('tgroup');
        $tgroup->setAttribute('cols', (string) $columns);
        $tgroup->append(
            "\n$strIndent  ",
            $thead,
            "\n$strIndent  ",
            $tbody,
            "\n$strIndent ",
        );

        $table = $doc->createElement('informaltable');
        $table->append(
            "\n$strIndent ",
            $tgroup,
            "\n$strIndent",
        );

        return $table;
    }

    private function getChangelogSection(DOMDocument $doc): DOMElement {
        $refSec = $this->generateRefSect1($doc, 'changelog');
        $headers = [
            $doc->createEntityReference('Version'),
            $doc->createEntityReference('Description'),
        ];
        $rows = [[
            new DOMText('8.X.0'),
            new DOMText("\n       Description\n      "),
        ]];
        $table = $this->generateDocbookInformalTable(
            $doc,
            /* indent: */ 2,
            /* columns: */ 2,
            /* headers: */ $headers,
            /* rows: */ $rows
        );
        $refSec->appendChild($table);

        $refSec->appendChild(new DOMText("\n "));
        return $refSec;
    }

    private function getExampleSection(DOMDocument $doc, string $id): DOMElement {
        $refSec = $this->generateRefSect1($doc, 'examples');

        $example = $doc->createElement('example');
        $fnName = $this->name->__toString();
        $example->setAttribute('xml:id', $id . '.example.basic');

        $title = $doc->createElement('title');
        $fn = $doc->createElement($this->isMethod() ? 'methodname' : 'function');
        $fn->append($fnName);
        $title->append($fn, ' example');

        $example->append("\n   ", $title);

        $para = $doc->createElement('simpara');
        $para->append("\n    ", "Description.", "\n   ");
        $example->append("\n   ", $para);

        $prog = $doc->createElement('programlisting');
        $prog->setAttribute('role', 'php');
        // So that GitHub syntax highlighting doesn't treat the closing tag
        // in the DOMCdataSection as indication that it should stop syntax
        // highlighting, break it up
        $empty = '';
        $code = new DOMCdataSection(
            <<<CODE_EXAMPLE

<?php
echo "Code example";
?$empty>

CODE_EXAMPLE
        );
        $prog->append("\n");
        $prog->appendChild($code);
        $prog->append("\n   ");

        $example->append("\n   ", $prog);
        $example->append("\n   ", $doc->createEntityReference('example.outputs'));

        $output = new DOMCdataSection(
            <<<OUPUT_EXAMPLE

Code example

OUPUT_EXAMPLE
        );
        $screen = $doc->createElement('screen');
        $screen->append("\n");
        $screen->appendChild($output);
        $screen->append("\n   ");

        $example->append(
            "\n   ",
            $screen,
            "\n  ",
        );

        $refSec->append(
            $example,
            "\n ",
        );
        return $refSec;
    }

    public function getMethodSynopsisElement(DOMDocument $doc): ?DOMElement {
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

        if ($this->isMethod()) {
            assert($this->name instanceof MethodName);
            $role = $doc->createAttribute("role");
            $role->value = addslashes($this->name->className->__toString());
            $methodSynopsis->appendChild($role);
        }

        $methodSynopsis->appendChild(new DOMText("\n   "));

        foreach ($this->attributes as $attribute) {
            $modifier = $doc->createElement("modifier", "#[\\" . $attribute->class . "]");
            $modifier->setAttribute("role", "attribute");
            $methodSynopsis->appendChild($modifier);
            $methodSynopsis->appendChild(new DOMText("\n   "));
        }

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
                foreach ($arg->attributes as $attribute) {
                    $attribute = $doc->createElement("modifier", "#[\\" . $attribute->class . "]");
                    $attribute->setAttribute("role", "attribute");

                    $methodparam->appendChild($attribute);
                }

                $methodparam->appendChild($arg->getMethodSynopsisType()->getTypeForDoc($doc));

                $parameter = $doc->createElement('parameter', $arg->name);
                if ($arg->sendBy !== ArgInfo::SEND_BY_VAL) {
                    $parameter->setAttribute("role", "reference");
                }

                $methodparam->appendChild($parameter);
                $defaultValue = $arg->getDefaultValueAsMethodSynopsisString();
                if ($defaultValue !== null) {
                    $initializer = $doc->createElement('initializer');
                    if (preg_match('/^[a-zA-Z_][a-zA-Z_0-9\:\\\\]*$/', $defaultValue)) {
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

    /** @param FuncInfo[] $generatedFuncInfos */
    public function findEquivalent(array $generatedFuncInfos): ?FuncInfo {
        foreach ($generatedFuncInfos as $generatedFuncInfo) {
            if ($generatedFuncInfo->equalsApartFromNameAndRefcount($this)) {
                return $generatedFuncInfo;
            }
        }
        return null;
    }

    public function toArgInfoCode(?int $minPHPCompatability): string {
        $code = $this->return->beginArgInfo(
            $this->getArgInfoName(),
            $this->numRequiredArgs,
            $minPHPCompatability === null || $minPHPCompatability >= PHP_81_VERSION_ID
        );

        foreach ($this->args as $argInfo) {
            $code .= $argInfo->toZendInfo();
        }

        $code .= "ZEND_END_ARG_INFO()";
        return $code . "\n";
    }

    public function __clone()
    {
        foreach ($this->args as $key => $argInfo) {
            $this->args[$key] = clone $argInfo;
        }
        $this->return = clone $this->return;
    }
}

class EvaluatedValue
{
    public /* readonly */ mixed $value;
    public SimpleType $type;
    public Expr $expr;
    public bool $isUnknownConstValue;
    /** @var ConstInfo[] */
    public array $originatingConsts;

    /**
     * @param array<string, ConstInfo> $allConstInfos
     */
    public static function createFromExpression(Expr $expr, ?SimpleType $constType, ?string $cConstName, array $allConstInfos): EvaluatedValue
    {
        // This visitor replaces the PHP constants by C constants. It allows direct expansion of the compiled constants, e.g. later in the pretty printer.
        $visitor = new class($allConstInfos) extends PhpParser\NodeVisitorAbstract
        {
            /** @var iterable<ConstInfo> */
            public array $visitedConstants = [];
            /** @var array<string, ConstInfo> */
            public array $allConstInfos;

            /** @param array<string, ConstInfo> $allConstInfos */
            public function __construct(array $allConstInfos)
            {
                $this->allConstInfos = $allConstInfos;
            }

            /** @return Node|null */
            public function enterNode(Node $expr)
            {
                if (!$expr instanceof Expr\ConstFetch && !$expr instanceof Expr\ClassConstFetch) {
                    return null;
                }

                if ($expr instanceof Expr\ClassConstFetch) {
                    $originatingConstName = new ClassConstName($expr->class, $expr->name->toString());
                } else {
                    $originatingConstName = new ConstName($expr->name->getAttribute('namespacedName'), $expr->name->toString());
                }

                if ($originatingConstName->isUnknown()) {
                    return null;
                }

                $const = $this->allConstInfos[$originatingConstName->__toString()] ?? null;
                if ($const !== null) {
                    $this->visitedConstants[] = $const;
                    return $const->getValue($this->allConstInfos)->expr;
                }
            }
        };

        $nodeTraverser = new PhpParser\NodeTraverser;
        $nodeTraverser->addVisitor($visitor);
        $expr = $nodeTraverser->traverse([$expr])[0];

        $isUnknownConstValue = false;

        $evaluator = new ConstExprEvaluator(
            static function (Expr $expr) use ($allConstInfos, &$isUnknownConstValue) {
                // $expr is a ConstFetch with a name of a C macro here
                if (!$expr instanceof Expr\ConstFetch) {
                    throw new Exception("Expression at line " . $expr->getStartLine() . " must be a global, non-magic constant");
                }

                $constName = $expr->name->__toString();
                if (strtolower($constName) === "unknown") {
                    $isUnknownConstValue = true;
                    return null;
                }

                foreach ($allConstInfos as $const) {
                    if ($constName != $const->cValue) {
                        continue;
                    }

                    $constType = ($const->phpDocType ?? $const->type)->tryToSimpleType();
                    if ($constType) {
                        if ($constType->isBool()) {
                            return true;
                        } elseif ($constType->isInt()) {
                            return 1;
                        } elseif ($constType->isFloat()) {
                            return M_PI;
                        } elseif ($constType->isString()) {
                            return $const->name;
                        } elseif ($constType->isArray()) {
                            return [];
                        }
                    }

                    return null;
                }

                throw new Exception("Constant " . $constName . " cannot be found");
            }
        );

        $result = $evaluator->evaluateDirectly($expr);

        return new EvaluatedValue(
            $result, // note: we are generally not interested in the actual value of $result, unless it's a bare value, without constants
            $constType ?? SimpleType::fromValue($result),
            $cConstName === null ? $expr : new Expr\ConstFetch(new Node\Name($cConstName)),
            $visitor->visitedConstants,
            $isUnknownConstValue
        );
    }

    public static function null(): EvaluatedValue
    {
        return new self(null, SimpleType::null(), new Expr\ConstFetch(new Node\Name('null')), [], false);
    }

    /**
     * @param mixed $value
     * @param ConstInfo[] $originatingConsts
     */
    private function __construct($value, SimpleType $type, Expr $expr, array $originatingConsts, bool $isUnknownConstValue)
    {
        $this->value = $value;
        $this->type = $type;
        $this->expr = $expr;
        $this->originatingConsts = $originatingConsts;
        $this->isUnknownConstValue = $isUnknownConstValue;
    }

    public function initializeZval(string $zvalName, bool $alreadyExists = false, string $forStringDef = ''): string
    {
        $cExpr = $this->getCExpr();

        $code = '';
        if (!$alreadyExists) {
            $code = "\tzval $zvalName;\n";
        }

        if ($this->type->isNull()) {
            $code .= "\tZVAL_NULL(&$zvalName);\n";
        } elseif ($this->type->isBool()) {
            if ($cExpr == 'true') {
                $code .= "\tZVAL_TRUE(&$zvalName);\n";
            } elseif ($cExpr == 'false') {
                $code .= "\tZVAL_FALSE(&$zvalName);\n";
            } else {
                $code .= "\tZVAL_BOOL(&$zvalName, $cExpr);\n";
            }
        } elseif ($this->type->isInt()) {
            $code .= "\tZVAL_LONG(&$zvalName, $cExpr);\n";
        } elseif ($this->type->isFloat()) {
            $code .= "\tZVAL_DOUBLE(&$zvalName, $cExpr);\n";
        } elseif ($this->type->isString()) {
            if ($cExpr === '""') {
                $code .= "\tZVAL_EMPTY_STRING(&$zvalName);\n";
            } else {
                if ($forStringDef === '') {
                    $forStringDef = "{$zvalName}_str";
                }
                $code .= "\tzend_string *$forStringDef = zend_string_init($cExpr, strlen($cExpr), 1);\n";
                $code .= "\tZVAL_STR(&$zvalName, $forStringDef);\n";
            }
        } elseif ($this->type->isArray()) {
            if ($cExpr == '[]') {
                $code .= "\tZVAL_EMPTY_ARRAY(&$zvalName);\n";
            } else {
                throw new Exception("Unimplemented default value");
            }
        } else {
            throw new Exception("Invalid default value: " . print_r($this->value, true) . ", type: " . print_r($this->type, true));
        }

        return $code;
    }

    public function getCExpr(): ?string
    {
        // $this->expr has all its PHP constants replaced by C constants
        $prettyPrinter = new Standard;
        $expr = $prettyPrinter->prettyPrintExpr($this->expr);
        // PHP single-quote to C double-quote string
        if ($this->type->isString()) {
            $expr = preg_replace("/(^'|'$)/", '"', $expr);
        }
        return $expr[0] == '"' ? $expr : preg_replace('(\bnull\b)', 'NULL', str_replace('\\', '', $expr));
    }
}

abstract class VariableLike
{
    protected int $flags;
    public ?Type $type;
    public /* readonly */ ?Type $phpDocType;
    private /* readonly */ ?string $link;
    protected ?int $phpVersionIdMinimumCompatibility;
    /** @var AttributeInfo[] */
    public array $attributes;
    protected /* readonly */ ?ExposedDocComment $exposedDocComment;

    /**
     * @param AttributeInfo[] $attributes
     */
    public function __construct(
        int $flags,
        ?Type $type,
        ?Type $phpDocType,
        ?string $link,
        ?int $phpVersionIdMinimumCompatibility,
        array $attributes,
        ?ExposedDocComment $exposedDocComment
    ) {
        $this->flags = $flags;
        $this->type = $type;
        $this->phpDocType = $phpDocType;
        $this->link = $link;
        $this->phpVersionIdMinimumCompatibility = $phpVersionIdMinimumCompatibility;
        $this->attributes = $attributes;
        $this->exposedDocComment = $exposedDocComment;
    }

    abstract protected function getVariableTypeName(): string;

    abstract protected function getFieldSynopsisDefaultLinkend(): string;

    abstract protected function getFieldSynopsisName(): string;

    /** @param array<string, ConstInfo> $allConstInfos */
    abstract protected function getFieldSynopsisValueString(array $allConstInfos): ?string;

    abstract public function discardInfoForOldPhpVersions(?int $minimumPhpVersionIdCompatibility): void;

    protected function getFlagsByPhpVersion(): VersionFlags
    {
        $flags = "ZEND_ACC_PUBLIC";
        if ($this->flags & Modifiers::PROTECTED) {
            $flags = "ZEND_ACC_PROTECTED";
        } elseif ($this->flags & Modifiers::PRIVATE) {
            $flags = "ZEND_ACC_PRIVATE";
        }

        return new VersionFlags([$flags]);
    }

    protected function getTypeCode(string $variableLikeName, string &$code): string
    {
        $variableLikeType = $this->getVariableTypeName();

        $typeCode = "";
        if ($this->type) {
            $arginfoType = $this->type->toArginfoType();
            if ($arginfoType->hasClassType()) {
                if (count($arginfoType->classTypes) >= 2) {
                    foreach ($arginfoType->classTypes as $classType) {
                        $escapedClassName = $classType->toEscapedName();
                        $varEscapedClassName = $classType->toVarEscapedName();
                        $code .= "\tzend_string *{$variableLikeType}_{$variableLikeName}_class_{$varEscapedClassName} = zend_string_init(\"{$escapedClassName}\", sizeof(\"{$escapedClassName}\") - 1, 1);\n";
                    }

                    $classTypeCount = count($arginfoType->classTypes);
                    $code .= "\tzend_type_list *{$variableLikeType}_{$variableLikeName}_type_list = malloc(ZEND_TYPE_LIST_SIZE($classTypeCount));\n";
                    $code .= "\t{$variableLikeType}_{$variableLikeName}_type_list->num_types = $classTypeCount;\n";

                    foreach ($arginfoType->classTypes as $k => $classType) {
                        $escapedClassName = $classType->toEscapedName();
                        $code .= "\t{$variableLikeType}_{$variableLikeName}_type_list->types[$k] = (zend_type) ZEND_TYPE_INIT_CLASS({$variableLikeType}_{$variableLikeName}_class_{$escapedClassName}, 0, 0);\n";
                    }

                    $typeMaskCode = $this->type->toArginfoType()->toTypeMask();

                    if ($this->type->isIntersection) {
                        $code .= "\tzend_type {$variableLikeType}_{$variableLikeName}_type = ZEND_TYPE_INIT_INTERSECTION({$variableLikeType}_{$variableLikeName}_type_list, $typeMaskCode);\n";
                    } else {
                        $code .= "\tzend_type {$variableLikeType}_{$variableLikeName}_type = ZEND_TYPE_INIT_UNION({$variableLikeType}_{$variableLikeName}_type_list, $typeMaskCode);\n";
                    }
                    $typeCode = "{$variableLikeType}_{$variableLikeName}_type";
                } else {
                    $escapedClassName = $arginfoType->classTypes[0]->toEscapedName();
                    $varEscapedClassName = $arginfoType->classTypes[0]->toVarEscapedName();
                    $code .= "\tzend_string *{$variableLikeType}_{$variableLikeName}_class_{$varEscapedClassName} = zend_string_init(\"{$escapedClassName}\", sizeof(\"{$escapedClassName}\")-1, 1);\n";

                    $typeCode = "(zend_type) ZEND_TYPE_INIT_CLASS({$variableLikeType}_{$variableLikeName}_class_{$varEscapedClassName}, 0, " . $arginfoType->toTypeMask() . ")";
                }
            } else {
                $typeCode = "(zend_type) ZEND_TYPE_INIT_MASK(" . $arginfoType->toTypeMask() . ")";
            }
        } else {
            $typeCode = "(zend_type) ZEND_TYPE_INIT_NONE(0)";
        }

        return $typeCode;
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function getFieldSynopsisElement(DOMDocument $doc, array $allConstInfos): DOMElement
    {
        $fieldsynopsisElement = $doc->createElement("fieldsynopsis");

        $this->addModifiersToFieldSynopsis($doc, $fieldsynopsisElement);

        $type = $this->phpDocType ?? $this->type;
        if ($type) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($type->getTypeForDoc($doc));
        }

        $varnameElement = $doc->createElement("varname", $this->getFieldSynopsisName());
        if ($this->link) {
            $varnameElement->setAttribute("linkend", $this->link);
        } else {
            $varnameElement->setAttribute("linkend", $this->getFieldSynopsisDefaultLinkend());
        }

        $fieldsynopsisElement->appendChild(new DOMText("\n     "));
        $fieldsynopsisElement->appendChild($varnameElement);

        $valueString = $this->getFieldSynopsisValueString($allConstInfos);
        if ($valueString) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $initializerElement = $doc->createElement("initializer",  $valueString);
            $fieldsynopsisElement->appendChild($initializerElement);
        }

        $fieldsynopsisElement->appendChild(new DOMText("\n    "));

        return $fieldsynopsisElement;
    }

    protected function addModifiersToFieldSynopsis(DOMDocument $doc, DOMElement $fieldsynopsisElement): void
    {
        if ($this->flags & Modifiers::PUBLIC) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "public"));
        } elseif ($this->flags & Modifiers::PROTECTED) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "protected"));
        } elseif ($this->flags & Modifiers::PRIVATE) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "private"));
        }
    }

}

class ConstInfo extends VariableLike
{
    public /* readonly */ AbstractConstName $name;
    public /* readonly */ Expr $value;
    private bool $isDeprecated;
    private /* readonly */ ?string $valueString;
    public /* readonly */ ?string $cond;
    public /* readonly */ ?string $cValue;
    public /* readonly */ bool $isUndocumentable;
    private /* readonly */ bool $isFileCacheAllowed;

    /**
     * @param AttributeInfo[] $attributes
     */
    public function __construct(
        AbstractConstName $name,
        int $flags,
        Expr $value,
        ?string $valueString,
        ?Type $type,
        ?Type $phpDocType,
        bool $isDeprecated,
        ?string $cond,
        ?string $cValue,
        bool $isUndocumentable,
        ?string $link,
        ?int $phpVersionIdMinimumCompatibility,
        array $attributes,
        ?ExposedDocComment $exposedDocComment,
        bool $isFileCacheAllowed
    ) {
        foreach ($attributes as $attr) {
            if ($attr->class === "Deprecated") {
                $isDeprecated = true;
                break;
            }
        }

        $this->name = $name;
        $this->value = $value;
        $this->valueString = $valueString;
        $this->isDeprecated = $isDeprecated;
        $this->cond = $cond;
        $this->cValue = $cValue;
        $this->isUndocumentable = $isUndocumentable;
        $this->isFileCacheAllowed = $isFileCacheAllowed;
        parent::__construct($flags, $type, $phpDocType, $link, $phpVersionIdMinimumCompatibility, $attributes, $exposedDocComment);
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function getValue(array $allConstInfos): EvaluatedValue
    {
        return EvaluatedValue::createFromExpression(
            $this->value,
            ($this->phpDocType ?? $this->type)->tryToSimpleType(),
            $this->cValue,
            $allConstInfos
        );
    }

    protected function getVariableTypeName(): string
    {
        return "constant";
    }

    protected function getFieldSynopsisDefaultLinkend(): string
    {
        $className = str_replace(["\\", "_"], ["-", "-"], $this->name->class->toLowerString());

        return "$className.constants." . strtolower(str_replace("_", "-", trim($this->name->getDeclarationName(), "_")));
    }

    protected function getFieldSynopsisName(): string
    {
        return $this->name->__toString();
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    protected function getFieldSynopsisValueString(array $allConstInfos): ?string
    {
        $value = EvaluatedValue::createFromExpression($this->value, null, $this->cValue, $allConstInfos);
        if ($value->isUnknownConstValue) {
            return null;
        }

        if ($value->originatingConsts) {
            return implode("\n", array_map(function (ConstInfo $const) use ($allConstInfos) {
                return $const->getFieldSynopsisValueString($allConstInfos);
            }, $value->originatingConsts));
        }

        return $this->valueString;
    }

    private function getPredefinedConstantElement(
        DOMDocument $doc,
        int $indentationLevel,
        string $name
    ): DOMElement {
        $indentation = str_repeat(" ", $indentationLevel);

        $element = $doc->createElement($name);

        $constantElement = $doc->createElement("constant");
        $constantElement->textContent = $this->name->__toString();

        $typeElement = ($this->phpDocType ?? $this->type)->getTypeForDoc($doc);

        $element->appendChild(new DOMText("\n$indentation "));
        $element->appendChild($constantElement);
        $element->appendChild(new DOMText("\n$indentation ("));
        $element->appendChild($typeElement);
        $element->appendChild(new DOMText(")\n$indentation"));

        return $element;
    }

    public function getPredefinedConstantTerm(DOMDocument $doc, int $indentationLevel): DOMElement {
        return $this->getPredefinedConstantElement($doc, $indentationLevel, "term");
    }

    public function getPredefinedConstantEntry(DOMDocument $doc, int $indentationLevel): DOMElement {
        return $this->getPredefinedConstantElement($doc, $indentationLevel, "entry");
    }

    public function discardInfoForOldPhpVersions(?int $phpVersionIdMinimumCompatibility): void {
        $this->type = null;
        $this->flags &= ~Modifiers::FINAL;
        $this->isDeprecated = false;
        $this->attributes = [];
        $this->phpVersionIdMinimumCompatibility = $phpVersionIdMinimumCompatibility;
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function getDeclaration(array $allConstInfos): string
    {
        $type = $this->phpDocType ?? $this->type;
        $simpleType = $type ? $type->tryToSimpleType() : null;
        if ($simpleType && $simpleType->name === "mixed") {
            $simpleType = null;
        }

        $value = EvaluatedValue::createFromExpression($this->value, $simpleType, $this->cValue, $allConstInfos);
        if ($value->isUnknownConstValue && ($simpleType === null || !$simpleType->isBuiltin)) {
            throw new Exception("Constant " . $this->name->__toString() . " must have a built-in PHPDoc type as the type couldn't be inferred from its value");
        }

        // i.e. const NAME = UNKNOWN;, without the annotation
        if ($value->isUnknownConstValue && $this->cValue === null && $value->expr instanceof Expr\ConstFetch && $value->expr->name->__toString() === "UNKNOWN") {
            throw new Exception("Constant " . $this->name->__toString() . " must have a @cvalue annotation");
        }

        // Condition will be added by generateCodeWithConditions()

        if ($this->name instanceof ClassConstName) {
            $code = $this->getClassConstDeclaration($value, $allConstInfos);
        } else {
            $code = $this->getGlobalConstDeclaration($value);
        }
        $code .= $this->getValueAssertion($value);

        return $code;
    }

    private function getGlobalConstDeclaration(EvaluatedValue $value): string
    {
        $constName = str_replace('\\', '\\\\', $this->name->__toString());
        $constValue = $value->value;
        $cExpr = $value->getCExpr();

        $flags = "CONST_PERSISTENT";
        if (!$this->isFileCacheAllowed) {
            $flags .= " | CONST_NO_FILE_CACHE";
        }
        if ($this->phpVersionIdMinimumCompatibility !== null && $this->phpVersionIdMinimumCompatibility < 80000) {
            $flags .= " | CONST_CS";
        }

        if ($this->isDeprecated) {
            $flags .= " | CONST_DEPRECATED";
        }

        $code = "\t";
        if ($this->attributes !== []) {
            if ($this->phpVersionIdMinimumCompatibility === null || $this->phpVersionIdMinimumCompatibility >= PHP_85_VERSION_ID) {
                // Registration only returns the constant since PHP 8.5, it's
                // not worth the bloat to add two different registration blocks
                // with conditions for the PHP version
                $constVarName = 'const_' . $constName;
                $code .= "zend_constant *$constVarName = ";
            }
        }

        if ($value->type->isNull()) {
            return $code . "REGISTER_NULL_CONSTANT(\"$constName\", $flags);\n";
        }

        if ($value->type->isBool()) {
            return $code . "REGISTER_BOOL_CONSTANT(\"$constName\", " . ($cExpr ?: ($constValue ? "true" : "false")) . ", $flags);\n";
        }

        if ($value->type->isInt()) {
            return $code . "REGISTER_LONG_CONSTANT(\"$constName\", " . ($cExpr ?: (int) $constValue) . ", $flags);\n";
        }

        if ($value->type->isFloat()) {
            return $code . "REGISTER_DOUBLE_CONSTANT(\"$constName\", " . ($cExpr ?: (float) $constValue) . ", $flags);\n";
        }

        if ($value->type->isString()) {
            return $code . "REGISTER_STRING_CONSTANT(\"$constName\", " . ($cExpr ?: '"' . addslashes($constValue) . '"') . ", $flags);\n";
        }

        throw new Exception("Unimplemented constant type");
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    private function getClassConstDeclaration(EvaluatedValue $value, array $allConstInfos): string
    {
        $constName = $this->name->getDeclarationName();

        // TODO $allConstInfos is unused
        $zvalCode = $value->initializeZval("const_{$constName}_value");

        $code = "\n" . $zvalCode;

        $code .= "\tzend_string *const_{$constName}_name = zend_string_init_interned(\"$constName\", sizeof(\"$constName\") - 1, 1);\n";
        $nameCode = "const_{$constName}_name";

        if ($this->exposedDocComment) {
            $commentCode = "const_{$constName}_comment";
            $escapedCommentInit = $this->exposedDocComment->getInitCode();
            $code .= "\tzend_string *$commentCode = $escapedCommentInit\n";
        } else {
            $commentCode = "NULL";
        }

        $php83MinimumCompatibility = $this->phpVersionIdMinimumCompatibility === null || $this->phpVersionIdMinimumCompatibility >= PHP_83_VERSION_ID;

        if ($this->type && !$php83MinimumCompatibility) {
            $code .= "#if (PHP_VERSION_ID >= " . PHP_83_VERSION_ID . ")\n";
        }

        if ($this->type) {
            $typeCode = $this->getTypeCode($constName, $code);

            if (!empty($this->attributes)) {
                $template = "\tzend_class_constant *const_" . $this->name->getDeclarationName() . " = ";
            } else {
                $template = "\t";
            }
            $template .= "zend_declare_typed_class_constant(class_entry, $nameCode, &const_{$constName}_value, %s, $commentCode, $typeCode);\n";

            $code .= $this->getFlagsByPhpVersion()->generateVersionDependentFlagCode(
                $template,
                $this->phpVersionIdMinimumCompatibility
            );
        }

        if ($this->type && !$php83MinimumCompatibility) {
            $code .= "#else\n";
        }

        if (!$this->type || !$php83MinimumCompatibility) {
            if (!empty($this->attributes)) {
                $template = "\tzend_class_constant *const_" . $this->name->getDeclarationName() . " = ";
            } else {
                $template = "\t";
            }
            $template .= "zend_declare_class_constant_ex(class_entry, $nameCode, &const_{$constName}_value, %s, $commentCode);\n";
            $code .= $this->getFlagsByPhpVersion()->generateVersionDependentFlagCode(
                $template,
                $this->phpVersionIdMinimumCompatibility
            );
        }

        if ($this->type && !$php83MinimumCompatibility) {
            $code .= "#endif\n";
        }

        $code .= "\tzend_string_release(const_{$constName}_name);\n";

        return $code;
    }

    private function getValueAssertion(EvaluatedValue $value): string
    {
        if ($value->isUnknownConstValue || $value->originatingConsts || $this->cValue === null) {
            return "";
        }

        $cExpr = $value->getCExpr();
        $constValue = $value->value;

        if ($value->type->isNull()) {
            return "\tZEND_ASSERT($cExpr == NULL);\n";
        }

        if ($value->type->isBool()) {
            $cValue = $constValue ? "true" : "false";
            return "\tZEND_ASSERT($cExpr == $cValue);\n";
        }

        if ($value->type->isInt()) {
            $cValue = (int) $constValue;
            return "\tZEND_ASSERT($cExpr == $cValue);\n";
        }

        if ($value->type->isFloat()) {
            $cValue = (float) $constValue;
            return "\tZEND_ASSERT($cExpr == $cValue);\n";
        }

        if ($value->type->isString()) {
            $cValue = '"' . addslashes($constValue) . '"';
            return "\tZEND_ASSERT(strcmp($cExpr, $cValue) == 0);\n";
        }

        throw new Exception("Unimplemented constant type");
    }

    protected function getFlagsByPhpVersion(): VersionFlags
    {
        $flags = parent::getFlagsByPhpVersion();

        // $this->isDeprecated also accounts for any #[\Deprecated] attributes
        if ($this->isDeprecated) {
            $flags->addForVersionsAbove("ZEND_ACC_DEPRECATED", PHP_80_VERSION_ID);
        }

        if ($this->flags & Modifiers::FINAL) {
            $flags->addForVersionsAbove("ZEND_ACC_FINAL", PHP_81_VERSION_ID);
        }

        return $flags;
    }

    protected function addModifiersToFieldSynopsis(DOMDocument $doc, DOMElement $fieldsynopsisElement): void
    {
        parent::addModifiersToFieldSynopsis($doc, $fieldsynopsisElement);

        if ($this->flags & Modifiers::FINAL) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "final"));
        }

        $fieldsynopsisElement->appendChild(new DOMText("\n     "));
        $fieldsynopsisElement->appendChild($doc->createElement("modifier", "const"));
    }
}

class StringBuilder {

    // Map possible variable names to the known string constant, see
    // ZEND_KNOWN_STRINGS
    private const PHP_80_KNOWN = [
        "file" => "ZEND_STR_FILE",
        "line" => "ZEND_STR_LINE",
        "function" => "ZEND_STR_FUNCTION",
        "class" => "ZEND_STR_CLASS",
        "object" => "ZEND_STR_OBJECT",
        "type" => "ZEND_STR_TYPE",
        // ZEND_STR_OBJECT_OPERATOR and ZEND_STR_PAAMAYIM_NEKUDOTAYIM are
        // not valid variable names
        "args" => "ZEND_STR_ARGS",
        "unknown" => "ZEND_STR_UNKNOWN",
        "eval" => "ZEND_STR_EVAL",
        "include" => "ZEND_STR_INCLUDE",
        "require" => "ZEND_STR_REQUIRE",
        "include_once" => "ZEND_STR_INCLUDE_ONCE",
        "require_once" => "ZEND_STR_REQUIRE_ONCE",
        "scalar" => "ZEND_STR_SCALAR",
        "error_reporting" => "ZEND_STR_ERROR_REPORTING",
        "static" => "ZEND_STR_STATIC",
        // ZEND_STR_THIS cannot be used since $this cannot be reassigned
        "value" => "ZEND_STR_VALUE",
        "key" => "ZEND_STR_KEY",
        "__invoke" => "ZEND_STR_MAGIC_INVOKE",
        "previous" => "ZEND_STR_PREVIOUS",
        "code" => "ZEND_STR_CODE",
        "message" => "ZEND_STR_MESSAGE",
        "severity" => "ZEND_STR_SEVERITY",
        "string" => "ZEND_STR_STRING",
        "trace" => "ZEND_STR_TRACE",
        "scheme" => "ZEND_STR_SCHEME",
        "host" => "ZEND_STR_HOST",
        "port" => "ZEND_STR_PORT",
        "user" => "ZEND_STR_USER",
        "pass" => "ZEND_STR_PASS",
        "path" => "ZEND_STR_PATH",
        "query" => "ZEND_STR_QUERY",
        "fragment" => "ZEND_STR_FRAGMENT",
        "NULL" => "ZEND_STR_NULL",
        "boolean" => "ZEND_STR_BOOLEAN",
        "integer" => "ZEND_STR_INTEGER",
        "double" => "ZEND_STR_DOUBLE",
        "array" => "ZEND_STR_ARRAY",
        "resource" => "ZEND_STR_RESOURCE",
        // ZEND_STR_CLOSED_RESOURCE has a space in it
        "name" => "ZEND_STR_NAME",
        // ZEND_STR_ARGV and ZEND_STR_ARGC are superglobals that wouldn't be
        // variable names
        "Array" => "ZEND_STR_ARRAY_CAPITALIZED",
        "bool" => "ZEND_STR_BOOL",
        "int" => "ZEND_STR_INT",
        "float" => "ZEND_STR_FLOAT",
        "callable" => "ZEND_STR_CALLABLE",
        "iterable" => "ZEND_STR_ITERABLE",
        "void" => "ZEND_STR_VOID",
        "false" => "ZEND_STR_FALSE",
        "null" => "ZEND_STR_NULL_LOWERCASE",
        "mixed" => "ZEND_STR_MIXED",
    ];

    // NEW in 8.1
    private const PHP_81_KNOWN = [
        "Unknown" => "ZEND_STR_UNKNOWN_CAPITALIZED",
        "never" => "ZEND_STR_NEVER",
        "__sleep" => "ZEND_STR_SLEEP",
        "__wakeup" => "ZEND_STR_WAKEUP",
        "cases" => "ZEND_STR_CASES",
        "from" => "ZEND_STR_FROM",
        "tryFrom" => "ZEND_STR_TRYFROM",
        "tryfrom" => "ZEND_STR_TRYFROM_LOWERCASE",
        // Omit ZEND_STR_AUTOGLOBAL_(SERVER|ENV|REQUEST)
    ];

    // NEW in 8.2
    private const PHP_82_KNOWN = [
        "true" => "ZEND_STR_TRUE",
        "Traversable" => "ZEND_STR_TRAVERSABLE",
        "count" => "ZEND_STR_COUNT",
        "SensitiveParameter" => "ZEND_STR_SENSITIVEPARAMETER",
    ];

    // Only new string in 8.3 is ZEND_STR_CONST_EXPR_PLACEHOLDER which is
    // not a valid variable name ("[constant expression]")

    // NEW in 8.4
    private const PHP_84_KNOWN = [
        "exit" => "ZEND_STR_EXIT",
        "Deprecated" => "ZEND_STR_DEPRECATED_CAPITALIZED",
        "since" => "ZEND_STR_SINCE",
        "get" => "ZEND_STR_GET",
        "set" => "ZEND_STR_SET",
    ];

    // NEW in 8.5
    private const PHP_85_KNOWN = [
        "self" => "ZEND_STR_SELF",
        "parent" => "ZEND_STR_PARENT",
        "username" => "ZEND_STR_USERNAME",
        "password" => "ZEND_STR_PASSWORD",
        "clone" => "ZEND_STR_CLONE",
        '8.0' => 'ZEND_STR_8_DOT_0',
        '8.1' => 'ZEND_STR_8_DOT_1',
        '8.2' => 'ZEND_STR_8_DOT_2',
        '8.3' => 'ZEND_STR_8_DOT_3',
        '8.4' => 'ZEND_STR_8_DOT_4',
        '8.5' => 'ZEND_STR_8_DOT_5',
    ];

    /**
     * Get an array of three strings:
     *   - declaration of zend_string, if needed, or empty otherwise
     *   - usage of that zend_string, or usage with ZSTR_KNOWN()
     *   - freeing the zend_string, if needed
     *
     * @param string $varName
     * @param string $strContent
     * @param ?int $minPHPCompatibility
     * @param bool $interned
     * @return string[]
     */
    public static function getString(
        string $varName,
        string $content,
        ?int $minPHPCompatibility,
        bool $interned = false
    ): array {
        // Generally strings will not be known
        $initFn = $interned ? 'zend_string_init_interned' : 'zend_string_init';
        $result = [
            "\tzend_string *$varName = $initFn(\"$content\", sizeof(\"$content\") - 1, 1);\n",
            $varName,
            "\tzend_string_release($varName);\n"
        ];
        // For attribute values that are not freed
        if ($varName === '') {
            $result[0] = "$initFn(\"$content\", sizeof(\"$content\") - 1, 1);\n";
        }
        // If not set, use the current latest version
        $allVersions = ALL_PHP_VERSION_IDS;
        $minPhp = $minPHPCompatibility ?? end($allVersions);
        if ($minPhp < PHP_80_VERSION_ID) {
            // No known strings in 7.0
            return $result;
        }
        $include = self::PHP_80_KNOWN;
        switch ($minPhp) {
            case PHP_85_VERSION_ID:
                $include = array_merge($include, self::PHP_85_KNOWN);
                // Intentional fall through

            case PHP_84_VERSION_ID:
                $include = array_merge($include, self::PHP_84_KNOWN);
                // Intentional fall through

            case PHP_83_VERSION_ID:
            case PHP_82_VERSION_ID:
                $include = array_merge($include, self::PHP_82_KNOWN);
                // Intentional fall through

            case PHP_81_VERSION_ID:
                $include = array_merge($include, self::PHP_81_KNOWN);
                break;
        }
        if (array_key_exists($content, $include)) {
            $knownStr = $include[$content];
            return [
                '',
                "ZSTR_KNOWN($knownStr)",
                '',
            ];
        }
        return $result;
    }
}

class PropertyInfo extends VariableLike
{
    private /* readonly */ int $classFlags;
    public /* readonly */ PropertyName $name;
    private /* readonly */ ?Expr $defaultValue;
    private /* readonly */ ?string $defaultValueString;
    private /* readonly */ bool $isDocReadonly;
    private /* readonly */ bool $isVirtual;

    /**
     * @param AttributeInfo[] $attributes
     */
    public function __construct(
        PropertyName $name,
        int $classFlags,
        int $flags,
        ?Type $type,
        ?Type $phpDocType,
        ?Expr $defaultValue,
        ?string $defaultValueString,
        bool $isDocReadonly,
        bool $isVirtual,
        ?string $link,
        ?int $phpVersionIdMinimumCompatibility,
        array $attributes,
        ?ExposedDocComment $exposedDocComment
    ) {
        $this->name = $name;
        $this->classFlags = $classFlags;
        $this->defaultValue = $defaultValue;
        $this->defaultValueString = $defaultValueString;
        $this->isDocReadonly = $isDocReadonly;
        $this->isVirtual = $isVirtual;
        parent::__construct($flags, $type, $phpDocType, $link, $phpVersionIdMinimumCompatibility, $attributes, $exposedDocComment);
    }

    protected function getVariableTypeName(): string
    {
        return "property";
    }

    protected function getFieldSynopsisDefaultLinkend(): string
    {
        $className = str_replace(["\\", "_"], ["-", "-"], $this->name->class->toLowerString());

        return "$className.props." . strtolower(str_replace("_", "-", trim($this->name->getDeclarationName(), "_")));
    }

    protected function getFieldSynopsisName(): string
    {
        return $this->name->getDeclarationName();
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    protected function getFieldSynopsisValueString(array $allConstInfos): ?string
    {
        return $this->defaultValueString;
    }

    public function discardInfoForOldPhpVersions(?int $phpVersionIdMinimumCompatibility): void {
        $this->type = null;
        $this->flags &= ~Modifiers::READONLY;
        $this->attributes = [];
        $this->phpVersionIdMinimumCompatibility = $phpVersionIdMinimumCompatibility;
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function getDeclaration(array $allConstInfos): string {
        $code = "\n";

        $propertyName = $this->name->getDeclarationName();

        if ($this->defaultValue === null) {
            $defaultValue = EvaluatedValue::null();
        } else {
            $defaultValue = EvaluatedValue::createFromExpression($this->defaultValue, null, null, $allConstInfos);
            if ($defaultValue->isUnknownConstValue || ($defaultValue->originatingConsts && $defaultValue->getCExpr() === null)) {
                echo "Skipping code generation for property $this->name, because it has an unknown constant default value\n";
                return "";
            }
        }

        $zvalName = "property_{$propertyName}_default_value";
        if ($this->defaultValue === null && $this->type !== null) {
            $code .= "\tzval $zvalName;\n\tZVAL_UNDEF(&$zvalName);\n";
        } else {
            $code .= $defaultValue->initializeZval($zvalName);
        }

        [$stringInit, $nameCode, $stringRelease] = StringBuilder::getString(
            "property_{$propertyName}_name",
            $propertyName,
            $this->phpVersionIdMinimumCompatibility
        );
        $code .= $stringInit;

        if ($this->exposedDocComment) {
            $commentCode = "property_{$propertyName}_comment";
            $escapedCommentInit = $this->exposedDocComment->getInitCode();
            $code .= "\tzend_string *$commentCode = $escapedCommentInit\n";
        } else {
            $commentCode = "NULL";
        }

        if (!empty($this->attributes)) {
            $template = "\tzend_property_info *property_" . $this->name->getDeclarationName() . " = ";
        } else {
            $template = "\t";
        }

        if ($this->phpVersionIdMinimumCompatibility === null || $this->phpVersionIdMinimumCompatibility >= PHP_80_VERSION_ID) {
            $typeCode = $this->getTypeCode($propertyName, $code);
            $template .= "zend_declare_typed_property(class_entry, $nameCode, &$zvalName, %s, $commentCode, $typeCode);\n";
        } else {
            $template .= "zend_declare_property_ex(class_entry, $nameCode, &$zvalName, %s, $commentCode);\n";
        }

        $code .= $this->getFlagsByPhpVersion()->generateVersionDependentFlagCode(
            $template,
            $this->phpVersionIdMinimumCompatibility
        );

        $code .= $stringRelease;

        return $code;
    }

    protected function getFlagsByPhpVersion(): VersionFlags
    {
        $flags = parent::getFlagsByPhpVersion();

        if ($this->flags & Modifiers::STATIC) {
            $flags->addForVersionsAbove("ZEND_ACC_STATIC", PHP_70_VERSION_ID);
        }

        if ($this->flags & Modifiers::FINAL) {
            $flags->addForVersionsAbove("ZEND_ACC_FINAL", PHP_84_VERSION_ID);
        }

        if ($this->flags & Modifiers::READONLY) {
            $flags->addForVersionsAbove("ZEND_ACC_READONLY", PHP_81_VERSION_ID);
        } elseif ($this->classFlags & Modifiers::READONLY) {
            $flags->addForVersionsAbove("ZEND_ACC_READONLY", PHP_82_VERSION_ID);
        }

        if ($this->isVirtual) {
            $flags->addForVersionsAbove("ZEND_ACC_VIRTUAL", PHP_84_VERSION_ID);
        }

        return $flags;
    }

    protected function addModifiersToFieldSynopsis(DOMDocument $doc, DOMElement $fieldsynopsisElement): void
    {
        parent::addModifiersToFieldSynopsis($doc, $fieldsynopsisElement);

        if ($this->flags & Modifiers::STATIC) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "static"));
        }

        if ($this->flags & Modifiers::FINAL) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "final"));
        }

        if ($this->flags & Modifiers::READONLY || $this->isDocReadonly) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "readonly"));
        }
    }
}

class EnumCaseInfo {
    private /* readonly */ string $name;
    private /* readonly */ ?Expr $value;

    public function __construct(string $name, ?Expr $value) {
        $this->name = $name;
        $this->value = $value;
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function getDeclaration(array $allConstInfos): string {
        $escapedName = addslashes($this->name);
        if ($this->value === null) {
            $code = "\n\tzend_enum_add_case_cstr(class_entry, \"$escapedName\", NULL);\n";
        } else {
            $value = EvaluatedValue::createFromExpression($this->value, null, null, $allConstInfos);

            $zvalName = "enum_case_{$escapedName}_value";
            $code = "\n" . $value->initializeZval($zvalName);
            $code .= "\tzend_enum_add_case_cstr(class_entry, \"$escapedName\", &$zvalName);\n";
        }

        return $code;
    }
}

// Instances of AttributeInfo are immutable and do not need to be cloned
// when held by an object that is cloned
class AttributeInfo {
    public /* readonly */ string $class;
    /** @var \PhpParser\Node\Arg[] */
    private /* readonly */ array $args;

    /** @param \PhpParser\Node\Arg[] $args */
    public function __construct(string $class, array $args) {
        $this->class = $class;
        $this->args = $args;
    }

    /**
     * @param array<string, ConstInfo> $allConstInfos
     * @param array<string, string> &$declaredStrings Map of string content to
     *   the name of a zend_string already created with that content
     */
    public function generateCode(string $invocation, string $nameSuffix, array $allConstInfos, ?int $phpVersionIdMinimumCompatibility, array &$declaredStrings = []): string {
        $escapedAttributeName = strtr($this->class, '\\', '_');
        [$stringInit, $nameCode, $stringRelease] = StringBuilder::getString(
            "attribute_name_{$escapedAttributeName}_$nameSuffix",
            addcslashes($this->class, "\\"),
            $phpVersionIdMinimumCompatibility,
            true
        );
        $code = "\n";
        $code .= $stringInit;
        $code .= "\t" . ($this->args ? "zend_attribute *attribute_{$escapedAttributeName}_$nameSuffix = " : "") . "$invocation, $nameCode, " . count($this->args) . ");\n";
        $code .= $stringRelease;

        foreach ($this->args as $i => $arg) {
            $initValue = '';
            if ($arg->value instanceof Node\Scalar\String_) {
                $strVal = $arg->value->value;
                [$strInit, $strUse, $strRelease] = StringBuilder::getString(
                    'unused',
                    $strVal,
                    $phpVersionIdMinimumCompatibility
                );
                if ($strInit === '') {
                    $initValue = "\tZVAL_STR(&attribute_{$escapedAttributeName}_{$nameSuffix}->args[$i].value, $strUse);\n";
                } elseif (isset($declaredStrings[$strVal])) {
                    $strUse = $declaredStrings[$strVal];
                    $initValue = "\tZVAL_STR_COPY(&attribute_{$escapedAttributeName}_{$nameSuffix}->args[$i].value, $strUse);\n";
                }
            }
            if ($initValue === '') {
                $value = EvaluatedValue::createFromExpression($arg->value, null, null, $allConstInfos);
                $code .= $value->initializeZval(
                    "attribute_{$escapedAttributeName}_{$nameSuffix}->args[$i].value",
                    true,
                    "attribute_{$escapedAttributeName}_{$nameSuffix}_arg{$i}_str"
                );
                if ($arg->value instanceof Node\Scalar\String_) {
                    $declaredStrings[$arg->value->value] = "attribute_{$escapedAttributeName}_{$nameSuffix}_arg{$i}_str";
                }
            } else {
                $code .= $initValue;
            }
            if ($arg->name) {
                [$stringInit, $nameCode, $stringRelease] = StringBuilder::getString(
                    "",
                    $arg->name->name,
                    $phpVersionIdMinimumCompatibility,
                    true
                );
                if ($stringInit === '') {
                    $nameCode .= ";\n";
                } else {
                    $nameCode = $stringInit;
                }
                $code .= "\tattribute_{$escapedAttributeName}_{$nameSuffix}->args[$i].name = $nameCode";
            }
        }
        return $code;
    }

    /**
     * @param AttributeGroup[] $attributeGroups
     * @return AttributeInfo[]
     */
    public static function createFromGroups(array $attributeGroups): array {
        $attributes = [];

        foreach ($attributeGroups as $attrGroup) {
            foreach ($attrGroup->attrs as $attr) {
                $attributes[] = new AttributeInfo($attr->name->toString(), $attr->args);
            }
        }

        return $attributes;
    }
}

class ClassInfo {
    public /* readonly */ Name $name;
    private int $flags;
    public string $type;
    public /* readonly */ ?string $alias;
    private /* readonly */ ?SimpleType $enumBackingType;
    private /* readonly */ bool $isDeprecated;
    private bool $isStrictProperties;
    /** @var AttributeInfo[] */
    private array $attributes;
    private ?ExposedDocComment $exposedDocComment;
    private bool $isNotSerializable;
    /** @var Name[] */
    private /* readonly */ array $extends;
    /** @var Name[] */
    private /* readonly */ array $implements;
    /** @var ConstInfo[] */
    public /* readonly */ array $constInfos;
    /** @var PropertyInfo[] */
    private /* readonly */ array $propertyInfos;
    /** @var FuncInfo[] */
    public array $funcInfos;
    /** @var EnumCaseInfo[] */
    private /* readonly */ array $enumCaseInfos;
    public /* readonly */ ?string $cond;
    public ?int $phpVersionIdMinimumCompatibility;
    public /* readonly */ bool $isUndocumentable;

    /**
     * @param AttributeInfo[] $attributes
     * @param Name[] $extends
     * @param Name[] $implements
     * @param ConstInfo[] $constInfos
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
        array $attributes,
        ?ExposedDocComment $exposedDocComment,
        bool $isNotSerializable,
        array $extends,
        array $implements,
        array $constInfos,
        array $propertyInfos,
        array $funcInfos,
        array $enumCaseInfos,
        ?string $cond,
        ?int $minimumPhpVersionIdCompatibility,
        bool $isUndocumentable
    ) {
        $this->name = $name;
        $this->flags = $flags;
        $this->type = $type;
        $this->alias = $alias;
        $this->enumBackingType = $enumBackingType;
        $this->isDeprecated = $isDeprecated;
        $this->isStrictProperties = $isStrictProperties;
        $this->attributes = $attributes;
        $this->exposedDocComment = $exposedDocComment;
        $this->isNotSerializable = $isNotSerializable;
        $this->extends = $extends;
        $this->implements = $implements;
        $this->constInfos = $constInfos;
        $this->propertyInfos = $propertyInfos;
        $this->funcInfos = $funcInfos;
        $this->enumCaseInfos = $enumCaseInfos;
        $this->cond = $cond;
        $this->phpVersionIdMinimumCompatibility = $minimumPhpVersionIdCompatibility;
        $this->isUndocumentable = $isUndocumentable;
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function getRegistration(array $allConstInfos): string
    {
        $params = [];
        foreach ($this->extends as $extends) {
            $params[] = "zend_class_entry *class_entry_" . implode("_", $extends->getParts());
        }
        foreach ($this->implements as $implements) {
            $params[] = "zend_class_entry *class_entry_" . implode("_", $implements->getParts());
        }

        $escapedName = implode("_", $this->name->getParts());

        $code = '';

        $php80MinimumCompatibility = $this->phpVersionIdMinimumCompatibility === null || $this->phpVersionIdMinimumCompatibility >= PHP_80_VERSION_ID;
        $php81MinimumCompatibility = $this->phpVersionIdMinimumCompatibility === null || $this->phpVersionIdMinimumCompatibility >= PHP_81_VERSION_ID;
        $php84MinimumCompatibility = $this->phpVersionIdMinimumCompatibility === null || $this->phpVersionIdMinimumCompatibility >= PHP_84_VERSION_ID;

        if ($this->type === "enum" && !$php81MinimumCompatibility) {
            $code .= "#if (PHP_VERSION_ID >= " . PHP_81_VERSION_ID . ")\n";
        }

        if ($this->cond) {
            $code .= "#if {$this->cond}\n";
        }

        $code .= "static zend_class_entry *register_class_$escapedName(" . (empty($params) ? "void" : implode(", ", $params)) . ")\n";

        $code .= "{\n";

        $flags = $this->getFlagsByPhpVersion();

        $classMethods = ($this->funcInfos === []) ? 'NULL' : "class_{$escapedName}_methods";
        if ($this->type === "enum") {
            $name = addslashes((string) $this->name);
            $backingType = $this->enumBackingType
                ? $this->enumBackingType->toTypeCode() : "IS_UNDEF";
            $code .= "\tzend_class_entry *class_entry = zend_register_internal_enum(\"$name\", $backingType, $classMethods);\n";
            if (!$flags->isEmpty()) {
                $code .= $this->getFlagsByPhpVersion()->generateVersionDependentFlagCode("\tclass_entry->ce_flags = %s;\n", $this->phpVersionIdMinimumCompatibility);
            }
        } else {
            $code .= "\tzend_class_entry ce, *class_entry;\n\n";
            if (count($this->name->getParts()) > 1) {
                $className = $this->name->getLast();
                $namespace = addslashes((string) $this->name->slice(0, -1));

                $code .= "\tINIT_NS_CLASS_ENTRY(ce, \"$namespace\", \"$className\", $classMethods);\n";
            } else {
                $code .= "\tINIT_CLASS_ENTRY(ce, \"$this->name\", $classMethods);\n";
            }

            if ($this->type === "class" || $this->type === "trait") {
                if (!$php84MinimumCompatibility) {
                    $code .= "#if (PHP_VERSION_ID >= " . PHP_84_VERSION_ID . ")\n";
                }

                $template = "\tclass_entry = zend_register_internal_class_with_flags(&ce, " . (isset($this->extends[0]) ? "class_entry_" . str_replace("\\", "_", $this->extends[0]->toString()) : "NULL") . ", %s);\n";
                $entries = $flags->generateVersionDependentFlagCode($template, $this->phpVersionIdMinimumCompatibility ? max($this->phpVersionIdMinimumCompatibility, PHP_84_VERSION_ID) : null);
                if ($entries !== '') {
                    $code .= $entries;
                } else {
                    $code .= sprintf($template, "0");
                }

                if (!$php84MinimumCompatibility) {
                    $code .= "#else\n";

                    $code .= "\tclass_entry = zend_register_internal_class_ex(&ce, " . (isset($this->extends[0]) ? "class_entry_" . str_replace("\\", "_", $this->extends[0]->toString()) : "NULL") . ");\n";
                    if (!$flags->isEmpty()) {
                        $code .= $flags->generateVersionDependentFlagCode("\tclass_entry->ce_flags |= %s;\n", $this->phpVersionIdMinimumCompatibility);
                    }
                    $code .= "#endif\n";
                }
            } else {
                $code .= "\tclass_entry = zend_register_internal_interface(&ce);\n";
                if (!$flags->isEmpty()) {
                    $code .= $flags->generateVersionDependentFlagCode("\tclass_entry->ce_flags |= %s;\n", $this->phpVersionIdMinimumCompatibility);
                }

            }
        }

        if ($this->exposedDocComment) {
            if (!$php84MinimumCompatibility) {
                $code .= "#if (PHP_VERSION_ID >= " . PHP_84_VERSION_ID . ")\n";
            }

            $code .= "\tclass_entry->doc_comment = " . $this->exposedDocComment->getInitCode() . "\n";

            if (!$php84MinimumCompatibility) {
                $code .= "#endif\n";
            }
        }

        $implements = array_map(
            function (Name $item) {
                return "class_entry_" . implode("_", $item->getParts());
            },
            $this->type === "interface" ? $this->extends : $this->implements
        );

        if (!empty($implements)) {
            $code .= "\tzend_class_implements(class_entry, " . count($implements) . ", " . implode(", ", $implements) . ");\n";
        }

        if ($this->alias) {
            $code .= "\tzend_register_class_alias(\"" . str_replace("\\", "\\\\", $this->alias) . "\", class_entry);\n";
        }

        $code .= generateCodeWithConditions(
            $this->constInfos,
            '',
            static fn (ConstInfo $const): string => $const->getDeclaration($allConstInfos)
        );

        foreach ($this->enumCaseInfos as $enumCase) {
            $code .= $enumCase->getDeclaration($allConstInfos);
        }

        foreach ($this->propertyInfos as $property) {
            $code .= $property->getDeclaration($allConstInfos);
        }
        // Reusable strings for wrapping conditional PHP 8.0+ code
        if ($php80MinimumCompatibility) {
            $php80CondStart = '';
            $php80CondEnd = '';
        } else {
            $php80CondStart = "\n#if (PHP_VERSION_ID >= " . PHP_80_VERSION_ID . ")";
            $php80CondEnd = "#endif\n";
        }

        $declaredStrings = [];

        if (!empty($this->attributes)) {
            $code .= $php80CondStart;

            foreach ($this->attributes as $key => $attribute) {
                $code .= $attribute->generateCode(
                    "zend_add_class_attribute(class_entry",
                    "class_{$escapedName}_$key",
                    $allConstInfos,
                    $this->phpVersionIdMinimumCompatibility,
                    $declaredStrings
                );
            }

            $code .= $php80CondEnd;
        }

        if ($attributeInitializationCode = generateConstantAttributeInitialization($this->constInfos, $allConstInfos, $this->phpVersionIdMinimumCompatibility, $this->cond, $declaredStrings)) {
            $code .= $php80CondStart;
            $code .= "\n" . $attributeInitializationCode;
            $code .= $php80CondEnd;
        }

        if ($attributeInitializationCode = generatePropertyAttributeInitialization($this->propertyInfos, $allConstInfos, $this->phpVersionIdMinimumCompatibility, $declaredStrings)) {
            $code .= $php80CondStart;
            $code .= "\n" . $attributeInitializationCode;
            $code .= $php80CondEnd;
        }

        if ($attributeInitializationCode = generateFunctionAttributeInitialization($this->funcInfos, $allConstInfos, $this->phpVersionIdMinimumCompatibility, $this->cond, $declaredStrings)) {
            $code .= $php80CondStart;
            $code .= "\n" . $attributeInitializationCode;
            $code .= $php80CondEnd;
        }

        $code .= "\n\treturn class_entry;\n";

        $code .= "}\n";

        if ($this->cond) {
            $code .= "#endif\n";
        }

        if ($this->type === "enum" && !$php81MinimumCompatibility) {
            $code .= "#endif\n";
        }

        return $code;
    }

    private function getFlagsByPhpVersion(): VersionFlags
    {
        $php70Flags = [];

        if ($this->type === "trait") {
            $php70Flags[] = "ZEND_ACC_TRAIT";
        }

        if ($this->flags & Modifiers::FINAL) {
            $php70Flags[] = "ZEND_ACC_FINAL";
        }

        if ($this->flags & Modifiers::ABSTRACT) {
            $php70Flags[] = "ZEND_ACC_ABSTRACT";
        }

        if ($this->isDeprecated) {
            $php70Flags[] = "ZEND_ACC_DEPRECATED";
        }

        $flags = new VersionFlags($php70Flags);

        if ($this->isStrictProperties) {
            $flags->addForVersionsAbove("ZEND_ACC_NO_DYNAMIC_PROPERTIES", PHP_80_VERSION_ID);
        }

        if ($this->isNotSerializable) {
            $flags->addForVersionsAbove("ZEND_ACC_NOT_SERIALIZABLE", PHP_81_VERSION_ID);
        }

        if ($this->flags & Modifiers::READONLY) {
            $flags->addForVersionsAbove("ZEND_ACC_READONLY_CLASS", PHP_82_VERSION_ID);
        }

        foreach ($this->attributes as $attr) {
            if ($attr->class === "AllowDynamicProperties") {
                $flags->addForVersionsAbove("ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES", PHP_82_VERSION_ID);
                break;
            }
        }

        return $flags;
    }

    public function discardInfoForOldPhpVersions(?int $phpVersionIdMinimumCompatibility): void {
        $this->attributes = [];
        $this->flags &= ~Modifiers::READONLY;
        $this->exposedDocComment = null;
        $this->isStrictProperties = false;
        $this->isNotSerializable = false;

        foreach ($this->propertyInfos as $propertyInfo) {
            $propertyInfo->discardInfoForOldPhpVersions($phpVersionIdMinimumCompatibility);
        }
        $this->phpVersionIdMinimumCompatibility = $phpVersionIdMinimumCompatibility;
    }

    /**
     * @param array<string, ClassInfo> $classMap
     * @param array<string, ConstInfo> $allConstInfos
     */
    public function getClassSynopsisDocument(array $classMap, array $allConstInfos): ?string {
        $doc = new DOMDocument();
        $doc->formatOutput = true;
        $classSynopsis = $this->getClassSynopsisElement($doc, $classMap, $allConstInfos);
        if (!$classSynopsis) {
            return null;
        }

        $doc->appendChild($classSynopsis);

        return $doc->saveXML();
    }

    /**
     * @param array<string, ClassInfo> $classMap
     * @param array<string, ConstInfo> $allConstInfos
     */
    public function getClassSynopsisElement(DOMDocument $doc, array $classMap, array $allConstInfos): ?DOMElement {
        $classSynopsis = $doc->createElement("classsynopsis");
        $classSynopsis->setAttribute("class", $this->type === "interface" ? "interface" : "class");

        $exceptionOverride = $this->type === "class" && $this->isException($classMap) ? "exception" : null;
        $ooElement = self::createOoElement($doc, $this, $exceptionOverride, true, null, 4);
        if (!$ooElement) {
            return null;
        }
        $classSynopsis->appendChild(new DOMText("\n    "));
        $classSynopsis->appendChild($ooElement);

        foreach ($this->extends as $k => $parent) {
            $parentInfo = $classMap[$parent->toString()] ?? null;
            if ($parentInfo === null) {
                throw new Exception("Missing parent class " . $parent->toString());
            }

            $ooElement = self::createOoElement(
                $doc,
                $parentInfo,
                null,
                false,
                $k === 0 ? "extends" : null,
                4
            );
            if (!$ooElement) {
                return null;
            }

            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsis->appendChild($ooElement);
        }

        foreach ($this->implements as $k => $interface) {
            $interfaceInfo = $classMap[$interface->toString()] ?? null;
            if (!$interfaceInfo) {
                throw new Exception("Missing implemented interface " . $interface->toString());
            }

            $ooElement = self::createOoElement($doc, $interfaceInfo, null, false, $k === 0 ? "implements" : null, 4);
            if (!$ooElement) {
                return null;
            }
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsis->appendChild($ooElement);
        }

        /** @var array<string, Name> $parentsWithInheritedConstants */
        $parentsWithInheritedConstants = [];
        /** @var array<string, Name> $parentsWithInheritedProperties */
        $parentsWithInheritedProperties = [];
        /** @var array<int, array{name: Name, types: int[]}> $parentsWithInheritedMethods */
        $parentsWithInheritedMethods = [];

        $this->collectInheritedMembers(
            $parentsWithInheritedConstants,
            $parentsWithInheritedProperties,
            $parentsWithInheritedMethods,
            $this->hasConstructor(),
            $classMap
        );

        $this->appendInheritedMemberSectionToClassSynopsis(
            $doc,
            $classSynopsis,
            $parentsWithInheritedConstants,
            "&Constants;",
            "&InheritedConstants;"
        );

        if (!empty($this->constInfos)) {
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "&Constants;");
            $classSynopsisInfo->setAttribute("role", "comment");
            $classSynopsis->appendChild($classSynopsisInfo);

            foreach ($this->constInfos as $constInfo) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $fieldSynopsisElement = $constInfo->getFieldSynopsisElement($doc, $allConstInfos);
                $classSynopsis->appendChild($fieldSynopsisElement);
            }
        }

        if (!empty($this->propertyInfos)) {
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "&Properties;");
            $classSynopsisInfo->setAttribute("role", "comment");
            $classSynopsis->appendChild($classSynopsisInfo);

            foreach ($this->propertyInfos as $propertyInfo) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $fieldSynopsisElement = $propertyInfo->getFieldSynopsisElement($doc, $allConstInfos);
                $classSynopsis->appendChild($fieldSynopsisElement);
            }
        }

        $this->appendInheritedMemberSectionToClassSynopsis(
            $doc,
            $classSynopsis,
            $parentsWithInheritedProperties,
            "&Properties;",
            "&InheritedProperties;"
        );

        if (!empty($this->funcInfos)) {
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "&Methods;");
            $classSynopsisInfo->setAttribute("role", "comment");
            $classSynopsis->appendChild($classSynopsisInfo);

            $classReference = self::getClassSynopsisReference($this->name);
            $escapedName = addslashes($this->name->__toString());

            if ($this->hasConstructor()) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $includeElement = $this->createIncludeElement(
                    $doc,
                    "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$classReference')/db:refentry/db:refsect1[@role='description']/descendant::db:constructorsynopsis[@role='$escapedName'])"
                );
                $classSynopsis->appendChild($includeElement);
            }

            if ($this->hasMethods()) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $includeElement = $this->createIncludeElement(
                    $doc,
                    "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$classReference')/db:refentry/db:refsect1[@role='description']/descendant::db:methodsynopsis[@role='$escapedName'])"
                );
                $classSynopsis->appendChild($includeElement);
            }

            if ($this->hasDestructor()) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $includeElement = $this->createIncludeElement(
                    $doc,
                    "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$classReference')/db:refentry/db:refsect1[@role='description']/descendant::db:destructorsynopsis[@role='$escapedName'])"
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
                $parentName = $parent["name"];
                $parentMethodsynopsisTypes = $parent["types"];

                $parentReference = self::getClassSynopsisReference($parentName);
                $escapedParentName = addslashes($parentName->__toString());

                foreach ($parentMethodsynopsisTypes as $parentMethodsynopsisType) {
                    $classSynopsis->appendChild(new DOMText("\n    "));
                    $includeElement = $this->createIncludeElement(
                        $doc,
                        "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$parentReference')/db:refentry/db:refsect1[@role='description']/descendant::db:{$parentMethodsynopsisType}[@role='$escapedParentName'])"
                    );

                    $classSynopsis->appendChild($includeElement);
                }
            }
        }

        $classSynopsis->appendChild(new DOMText("\n   "));

        return $classSynopsis;
    }

    private static function createOoElement(
        DOMDocument $doc,
        ClassInfo $classInfo,
        ?string $typeOverride,
        bool $withModifiers,
        ?string $modifierOverride,
        int $indentationLevel
    ): ?DOMElement {
        $indentation = str_repeat(" ", $indentationLevel);

        if ($classInfo->type !== "class" && $classInfo->type !== "interface") {
            echo "Class synopsis generation is not implemented for " . $classInfo->type . "\n";
            return null;
        }

        $type = $typeOverride !== null ? $typeOverride : $classInfo->type;

        $ooElement = $doc->createElement("oo$type");
        $ooElement->appendChild(new DOMText("\n$indentation "));
        if ($modifierOverride !== null) {
            $ooElement->appendChild($doc->createElement('modifier', $modifierOverride));
            $ooElement->appendChild(new DOMText("\n$indentation "));
        } elseif ($withModifiers) {
            foreach ($classInfo->attributes as $attribute) {
                $modifier = $doc->createElement("modifier", "#[\\" . $attribute->class . "]");
                $modifier->setAttribute("role", "attribute");
                $ooElement->appendChild($modifier);
                $ooElement->appendChild(new DOMText("\n$indentation "));
            }

            if ($classInfo->flags & Modifiers::FINAL) {
                $ooElement->appendChild($doc->createElement('modifier', 'final'));
                $ooElement->appendChild(new DOMText("\n$indentation "));
            }
            if ($classInfo->flags & Modifiers::ABSTRACT) {
                $ooElement->appendChild($doc->createElement('modifier', 'abstract'));
                $ooElement->appendChild(new DOMText("\n$indentation "));
            }
            if ($classInfo->flags & Modifiers::READONLY) {
                $ooElement->appendChild($doc->createElement('modifier', 'readonly'));
                $ooElement->appendChild(new DOMText("\n$indentation "));
            }
        }

        $nameElement = $doc->createElement("{$type}name", $classInfo->name->toString());
        $ooElement->appendChild($nameElement);
        $ooElement->appendChild(new DOMText("\n$indentation"));

        return $ooElement;
    }

    public static function getClassSynopsisFilename(Name $name): string {
        return strtolower(str_replace("_", "-", implode('-', $name->getParts())));
    }

    private static function getClassSynopsisReference(Name $name): string {
        return "class." . self::getClassSynopsisFilename($name);
    }

    /**
     * @param array<string, Name> $parentsWithInheritedConstants
     * @param array<string, Name> $parentsWithInheritedProperties
     * @param array<string, array{name: Name, types: int[]}> $parentsWithInheritedMethods
     * @param array<string, ClassInfo> $classMap
     */
    private function collectInheritedMembers(
        array &$parentsWithInheritedConstants,
        array &$parentsWithInheritedProperties,
        array &$parentsWithInheritedMethods,
        bool $hasConstructor,
        array $classMap
    ): void {
        foreach ($this->extends as $parent) {
            $parentInfo = $classMap[$parent->toString()] ?? null;
            $parentName = $parent->toString();

            if (!$parentInfo) {
                throw new Exception("Missing parent class $parentName");
            }

            if (!empty($parentInfo->constInfos) && !isset($parentsWithInheritedConstants[$parentName])) {
                $parentsWithInheritedConstants[] = $parent;
            }

            if (!empty($parentInfo->propertyInfos) && !isset($parentsWithInheritedProperties[$parentName])) {
                $parentsWithInheritedProperties[$parentName] = $parent;
            }

            if (!$hasConstructor && $parentInfo->hasNonPrivateConstructor()) {
                $parentsWithInheritedMethods[$parentName]["name"] = $parent;
                $parentsWithInheritedMethods[$parentName]["types"][] = "constructorsynopsis";
            }

            if ($parentInfo->hasMethods()) {
                $parentsWithInheritedMethods[$parentName]["name"] = $parent;
                $parentsWithInheritedMethods[$parentName]["types"][] = "methodsynopsis";
            }

            if ($parentInfo->hasDestructor()) {
                $parentsWithInheritedMethods[$parentName]["name"] = $parent;
                $parentsWithInheritedMethods[$parentName]["types"][] = "destructorsynopsis";
            }

            $parentInfo->collectInheritedMembers(
                $parentsWithInheritedConstants,
                $parentsWithInheritedProperties,
                $parentsWithInheritedMethods,
                $hasConstructor,
                $classMap
            );
        }

        foreach ($this->implements as $parent) {
            $parentInfo = $classMap[$parent->toString()] ?? null;
            if (!$parentInfo) {
                throw new Exception("Missing parent interface " . $parent->toString());
            }

            if (!empty($parentInfo->constInfos) && !isset($parentsWithInheritedConstants[$parent->toString()])) {
                $parentsWithInheritedConstants[$parent->toString()] = $parent;
            }

            $unusedParentsWithInheritedProperties = [];
            $unusedParentsWithInheritedMethods = [];

            $parentInfo->collectInheritedMembers(
                $parentsWithInheritedConstants,
                $unusedParentsWithInheritedProperties,
                $unusedParentsWithInheritedMethods,
                $hasConstructor,
                $classMap
            );
        }
    }

    /** @param array<string, ClassInfo> $classMap */
    private function isException(array $classMap): bool
    {
        if ($this->name->toString() === "Throwable") {
            return true;
        }

        foreach ($this->extends as $parentName) {
            $parent = $classMap[$parentName->toString()] ?? null;
            if ($parent === null) {
                throw new Exception("Missing parent class " . $parentName->toString());
            }

            if ($parent->isException($classMap)) {
                return true;
            }
        }

        if ($this->type === "class") {
            foreach ($this->implements as $interfaceName) {
                $interface = $classMap[$interfaceName->toString()] ?? null;
                if ($interface === null) {
                    throw new Exception("Missing implemented interface " . $interfaceName->toString());
                }

                if ($interface->isException($classMap)) {
                    return true;
                }
            }
        }

        return false;
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

    private function hasNonPrivateConstructor(): bool
    {
        foreach ($this->funcInfos as $funcInfo) {
            if ($funcInfo->name->isConstructor() && !($funcInfo->flags & Modifiers::PRIVATE)) {
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
        foreach ($this->constInfos as $key => $constInfo) {
            $this->constInfos[$key] = clone $constInfo;
        }

        foreach ($this->propertyInfos as $key => $propertyInfo) {
            $this->propertyInfos[$key] = clone $propertyInfo;
        }

        foreach ($this->funcInfos as $key => $funcInfo) {
            $this->funcInfos[$key] = clone $funcInfo;
        }
    }

    /**
     * @param Name[] $parents
     */
    private function appendInheritedMemberSectionToClassSynopsis(DOMDocument $doc, DOMElement $classSynopsis, array $parents, string $label, string $inheritedLabel): void
    {
        if (empty($parents)) {
            return;
        }

        $classSynopsis->appendChild(new DOMText("\n\n    "));
        $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "$inheritedLabel");
        $classSynopsisInfo->setAttribute("role", "comment");
        $classSynopsis->appendChild($classSynopsisInfo);

        foreach ($parents as $parent) {
            $classSynopsis->appendChild(new DOMText("\n    "));
            $parentReference = self::getClassSynopsisReference($parent);

            $includeElement = $this->createIncludeElement(
                $doc,
                "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$parentReference')/db:partintro/db:section/db:classsynopsis/db:fieldsynopsis[preceding-sibling::db:classsynopsisinfo[1][@role='comment' and text()='$label']]))"
            );
            $classSynopsis->appendChild($includeElement);
        }
    }
}

class FileInfo {
    /** @var string[] */
    public array $dependencies = [];
    /** @var ConstInfo[] */
    public array $constInfos = [];
    /** @var FuncInfo[] */
    public array $funcInfos = [];
    /** @var ClassInfo[] */
    public array $classInfos = [];
    public bool $generateFunctionEntries = false;
    public string $declarationPrefix = "";
    public bool $generateClassEntries = false;
    private bool $isUndocumentable = false;
    private bool $legacyArginfoGeneration = false;
    private ?int $minimumPhpVersionIdCompatibility = null;

    /** @param array<int, DocCommentTag> $fileTags */
    public function __construct(array $fileTags) {
        foreach ($fileTags as $tag) {
            if ($tag->name === 'generate-function-entries') {
                $this->generateFunctionEntries = true;
                $this->declarationPrefix = $tag->value ? $tag->value . " " : "";
            } else if ($tag->name === 'generate-legacy-arginfo') {
                if ($tag->value && !in_array((int) $tag->value, ALL_PHP_VERSION_IDS, true)) {
                    throw new Exception(
                        "Legacy PHP version must be one of: \"" . PHP_70_VERSION_ID . "\" (PHP 7.0), \"" . PHP_80_VERSION_ID . "\" (PHP 8.0), " .
                        "\"" . PHP_81_VERSION_ID . "\" (PHP 8.1), \"" . PHP_82_VERSION_ID . "\" (PHP 8.2), \"" . PHP_83_VERSION_ID . "\" (PHP 8.3), " .
                        "\"" . PHP_84_VERSION_ID . "\" (PHP 8.4), \"" . PHP_85_VERSION_ID . "\" (PHP 8.5), \"" . $tag->value . "\" provided"
                    );
                }

                $this->minimumPhpVersionIdCompatibility = ($tag->value ? (int) $tag->value : PHP_70_VERSION_ID);
            } else if ($tag->name === 'generate-class-entries') {
                $this->generateClassEntries = true;
                $this->declarationPrefix = $tag->value ? $tag->value . " " : "";
            } else if ($tag->name === 'undocumentable') {
                $this->isUndocumentable = true;
            }
        }

        // Generating class entries require generating function/method entries
        if ($this->generateClassEntries && !$this->generateFunctionEntries) {
            $this->generateFunctionEntries = true;
        }
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

    /** @return array<string, ConstInfo> */
    public function getAllConstInfos(): array {
        $result = [];

        foreach ($this->constInfos as $constInfo) {
            $result[$constInfo->name->__toString()] = $constInfo;
        }

        foreach ($this->classInfos as $classInfo) {
            foreach ($classInfo->constInfos as $constInfo) {
                $result[$constInfo->name->__toString()] = $constInfo;
            }
        }

        return $result;
    }

    public function __clone()
    {
        foreach ($this->constInfos as $key => $constInfo) {
            $this->constInfos[$key] = clone $constInfo;
        }

        foreach ($this->funcInfos as $key => $funcInfo) {
            $this->funcInfos[$key] = clone $funcInfo;
        }

        foreach ($this->classInfos as $key => $classInfo) {
            $this->classInfos[$key] = clone $classInfo;
        }
    }

    public function getMinimumPhpVersionIdCompatibility(): ?int {
        // Non-legacy arginfo files are always PHP 8.0+ compatible
        if (!$this->legacyArginfoGeneration &&
            $this->minimumPhpVersionIdCompatibility !== null &&
            $this->minimumPhpVersionIdCompatibility < PHP_80_VERSION_ID
        ) {
            return PHP_80_VERSION_ID;
        }

        return $this->minimumPhpVersionIdCompatibility;
    }

    public function shouldGenerateLegacyArginfo(): bool {
        return $this->minimumPhpVersionIdCompatibility !== null && $this->minimumPhpVersionIdCompatibility < PHP_80_VERSION_ID;
    }

    public function getLegacyVersion(): FileInfo {
        $legacyFileInfo = clone $this;
        $legacyFileInfo->legacyArginfoGeneration = true;
        $phpVersionIdMinimumCompatibility = $legacyFileInfo->getMinimumPhpVersionIdCompatibility();

        foreach ($legacyFileInfo->getAllFuncInfos() as $funcInfo) {
            $funcInfo->discardInfoForOldPhpVersions($phpVersionIdMinimumCompatibility);
        }
        foreach ($legacyFileInfo->classInfos as $classInfo) {
            $classInfo->discardInfoForOldPhpVersions($phpVersionIdMinimumCompatibility);
        }
        foreach ($legacyFileInfo->getAllConstInfos() as $constInfo) {
            $constInfo->discardInfoForOldPhpVersions($phpVersionIdMinimumCompatibility);
        }
        return $legacyFileInfo;
    }

    public static function parseStubFile(string $code): FileInfo {
        $parser = new PhpParser\Parser\Php7(new PhpParser\Lexer\Emulative());
        $nodeTraverser = new PhpParser\NodeTraverser;
        $nodeTraverser->addVisitor(new PhpParser\NodeVisitor\NameResolver);
        $prettyPrinter = new class extends Standard {
            protected function pName_FullyQualified(Name\FullyQualified $node): string {
                return implode('\\', $node->getParts());
            }
        };

        $stmts = $parser->parse($code);
        $nodeTraverser->traverse($stmts);

        $fileTags = DocCommentTag::parseDocComments(self::getFileDocComments($stmts));
        $fileInfo = new FileInfo($fileTags);

        $fileInfo->handleStatements($stmts, $prettyPrinter);
        return $fileInfo;
    }

    /** @return DocComment[] */
    private static function getFileDocComments(array $stmts): array {
        if (empty($stmts)) {
            return [];
        }

        return array_filter(
            $stmts[0]->getComments(),
            static fn ($comment): bool => $comment instanceof DocComment
        );
    }

    private function handleStatements(array $stmts, PrettyPrinterAbstract $prettyPrinter): void {
        $conds = [];
        foreach ($stmts as $stmt) {
            $cond = self::handlePreprocessorConditions($conds, $stmt);

            if ($stmt instanceof Stmt\Nop) {
                continue;
            }

            if ($stmt instanceof Stmt\Namespace_) {
                $this->handleStatements($stmt->stmts, $prettyPrinter);
                continue;
            }

            if ($stmt instanceof Stmt\Const_) {
                foreach ($stmt->consts as $const) {
                    $this->constInfos[] = parseConstLike(
                        $prettyPrinter,
                        new ConstName($const->namespacedName, $const->name->toString()),
                        $const,
                        0,
                        null,
                        $stmt->getComments(),
                        $cond,
                        $this->isUndocumentable,
                        $this->getMinimumPhpVersionIdCompatibility(),
                        AttributeInfo::createFromGroups($stmt->attrGroups)
                    );
                }
                continue;
            }

            if ($stmt instanceof Stmt\Function_) {
                $this->funcInfos[] = parseFunctionLike(
                    $prettyPrinter,
                    new FunctionName($stmt->namespacedName),
                    0,
                    0,
                    $stmt,
                    $cond,
                    $this->isUndocumentable,
                    $this->getMinimumPhpVersionIdCompatibility()
                );
                continue;
            }

            if ($stmt instanceof Stmt\ClassLike) {
                $className = $stmt->namespacedName;
                $constInfos = [];
                $propertyInfos = [];
                $methodInfos = [];
                $enumCaseInfos = [];
                foreach ($stmt->stmts as $classStmt) {
                    $cond = self::handlePreprocessorConditions($conds, $classStmt);
                    if ($classStmt instanceof Stmt\Nop) {
                        continue;
                    }

                    $classFlags = $stmt instanceof Class_ ? $stmt->flags : 0;
                    $abstractFlag = $stmt instanceof Stmt\Interface_ ? Modifiers::ABSTRACT : 0;

                    if ($classStmt instanceof Stmt\ClassConst) {
                        foreach ($classStmt->consts as $const) {
                            $constInfos[] = parseConstLike(
                                $prettyPrinter,
                                new ClassConstName($className, $const->name->toString()),
                                $const,
                                $classStmt->flags,
                                $classStmt->type,
                                $classStmt->getComments(),
                                $cond,
                                $this->isUndocumentable,
                                $this->getMinimumPhpVersionIdCompatibility(),
                                AttributeInfo::createFromGroups($classStmt->attrGroups)
                            );
                        }
                    } else if ($classStmt instanceof Stmt\Property) {
                        if (!($classStmt->flags & Class_::VISIBILITY_MODIFIER_MASK)) {
                            throw new Exception("Visibility modifier is required");
                        }
                        foreach ($classStmt->props as $property) {
                            $propertyInfos[] = parseProperty(
                                $className,
                                $classFlags,
                                $classStmt->flags,
                                $property,
                                $classStmt->type,
                                $classStmt->getComments(),
                                $prettyPrinter,
                                $this->getMinimumPhpVersionIdCompatibility(),
                                AttributeInfo::createFromGroups($classStmt->attrGroups)
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
                            $cond,
                            $this->isUndocumentable,
                            $this->getMinimumPhpVersionIdCompatibility()
                        );
                    } else if ($classStmt instanceof Stmt\EnumCase) {
                        $enumCaseInfos[] = new EnumCaseInfo(
                            $classStmt->name->toString(), $classStmt->expr);
                    } else {
                        throw new Exception("Not implemented {$classStmt->getType()}");
                    }
                }

                $this->classInfos[] = parseClass(
                    $className,
                    $stmt,
                    $constInfos,
                    $propertyInfos,
                    $methodInfos,
                    $enumCaseInfos,
                    $cond,
                    $this->getMinimumPhpVersionIdCompatibility(),
                    $this->isUndocumentable
                );
                continue;
            }

            if ($stmt instanceof Stmt\Expression) {
                $expr = $stmt->expr;
                if ($expr instanceof Expr\Include_) {
                    $this->dependencies[] = (string)EvaluatedValue::createFromExpression($expr->expr, null, null, [])->value;
                    continue;
                }
            }

            throw new Exception("Unexpected node {$stmt->getType()}");
        }
        if (!empty($conds)) {
            throw new Exception("Unterminated preprocessor conditions");
        }
    }

    private static function handlePreprocessorConditions(array &$conds, Stmt $stmt): ?string {
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

    /** @param array<string, ConstInfo> $allConstInfos */
    public function generateClassEntryCode(array $allConstInfos): string {
        $code = "";

        foreach ($this->classInfos as $class) {
            $code .= "\n" . $class->getRegistration($allConstInfos);
        }

        return $code;
    }
}

class DocCommentTag {
    public /* readonly */ string $name;
    public /* readonly */ ?string $value;

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
            preg_match('/^\s*([\w\|\\\\\[\]<>, ]+)\s*(?:[{(]|(\.\.\.)?\$\w+).*$/', $value, $matches);
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
            // Allow for parsing extended types like callable(string):mixed in docblocks
            preg_match('/^\s*(?<type>[\w\|\\\\]+(?<parens>\((?<inparens>(?:(?&parens)|[^(){}[\]<>]*+))++\)|\{(?&inparens)\}|\[(?&inparens)\]|<(?&inparens)>)*+(?::(?&type))?)\s*(\.\.\.)?\$(?<name>\w+).*$/', $value, $matches);
        } elseif ($this->name === "prefer-ref") {
            preg_match('/^\s*\$(?<name>\w+).*$/', $value, $matches);
        }

        if (!isset($matches["name"])) {
            throw new Exception("@$this->name doesn't contain a variable name or has an invalid format \"$value\"");
        }

        return $matches["name"];
    }

    /** @return DocCommentTag[] */
    public static function parseDocComments(array $comments): array {
        $tags = [];
        foreach ($comments as $comment) {
            if (!($comment instanceof DocComment)) {
                continue;
            }
            $commentText = substr($comment->getText(), 2, -2);
            foreach (explode("\n", $commentText) as $commentLine) {
                $regex = '/^\*\s*@([a-z-]+)(?:\s+(.+))?$/';
                if (preg_match($regex, trim($commentLine), $matches)) {
                    $tags[] = new DocCommentTag($matches[1], $matches[2] ?? null);
                }
            }
        }

        return $tags;
    }

    /**
     * @param DocCommentTag[] $tags
     * @return array<string, ?string> Mapping tag names to the value (or null),
     *   if a tag is present multiple times the last value is used
     */
    public static function makeTagMap(array $tags): array {
        $map = [];
        foreach ($tags as $tag) {
            $map[$tag->name] = $tag->value;
        }
        return $map;
    }
}

// Instances of ExposedDocComment are immutable and do not need to be cloned
// when held by an object that is cloned
class ExposedDocComment {
    private /* readonly */ string $docComment;

    public function __construct(string $docComment) {
        $this->docComment = $docComment;
    }

    public function escape(): string {
        return str_replace("\n", '\n', addslashes($this->docComment));
    }

    public function getInitCode(): string {
        return "zend_string_init_interned(\"" . $this->escape() . "\", " . strlen($this->docComment) . ", 1);";
    }

    /** @param array<int, DocComment> $comments */
    public static function extractExposedComment(array $comments): ?ExposedDocComment {
        $exposedDocComment = null;

        foreach ($comments as $comment) {
            $text = $comment->getText();
            $matches = [];
            $pattern = "#^(\s*\/\*\*)(\s*@genstubs-expose-comment-block)(\s*)$#m";

            if (preg_match($pattern, $text, $matches) !== 1) {
                continue;
            }

            if ($exposedDocComment !== null) {
                throw new Exception("Only one PHPDoc comment block can be exposed");
            }

            $exposedDocComment = preg_replace($pattern, '$1$3', $text);
        }

        return $exposedDocComment ? new ExposedDocComment($exposedDocComment) : null;
    }
}

// Instances of FramelessFunctionInfo are immutable and do not need to be cloned
// when held by an object that is cloned
class FramelessFunctionInfo {
    public /* readonly */ int $arity;

    public function __construct(string $json) {
        // FIXME: Should have some validation
        $json = json_decode($json, true);

        $this->arity = $json["arity"];
    }
}

function parseFunctionLike(
    PrettyPrinterAbstract $prettyPrinter,
    FunctionOrMethodName $name,
    int $classFlags,
    int $flags,
    Node\FunctionLike $func,
    ?string $cond,
    bool $isUndocumentable,
    ?int $minimumPhpVersionIdCompatibility
): FuncInfo {
    try {
        $comments = $func->getComments();
        $paramMeta = [];
        $aliasType = null;
        $alias = null;
        $isDeprecated = false;
        $supportsCompileTimeEval = false;
        $verify = true;
        $docReturnType = null;
        $tentativeReturnType = false;
        $docParamTypes = [];
        $refcount = null;
        $framelessFunctionInfos = [];

        if ($comments) {
            $tags = DocCommentTag::parseDocComments($comments);
            $tagMap = DocCommentTag::makeTagMap($tags);

            $isDeprecated = array_key_exists('deprecated', $tagMap);
            $verify = !array_key_exists('no-verify', $tagMap);
            $tentativeReturnType = array_key_exists('tentative-return-type', $tagMap);
            $supportsCompileTimeEval = array_key_exists('compile-time-eval', $tagMap);
            $isUndocumentable = $isUndocumentable || array_key_exists('undocumentable', $tagMap);

            foreach ($tags as $tag) {
                switch ($tag->name) {
                    case 'alias':
                    case 'implementation-alias':
                        $aliasType = $tag->name;
                        $aliasParts = explode("::", $tag->getValue());
                        if (count($aliasParts) === 1) {
                            $alias = new FunctionName(new Name($aliasParts[0]));
                        } else {
                            $alias = new MethodName(new Name($aliasParts[0]), $aliasParts[1]);
                        }
                        break;

                    case 'return':
                        $docReturnType = $tag->getType();
                        break;

                    case 'param':
                        $docParamTypes[$tag->getVariableName()] = $tag->getType();
                        break;

                    case 'refcount':
                        $refcount = $tag->getValue();
                        break;

                    case 'prefer-ref':
                        $varName = $tag->getVariableName();
                        if (!isset($paramMeta[$varName])) {
                            $paramMeta[$varName] = [];
                        }
                        $paramMeta[$varName][$tag->name] = true;
                        break;

                    case 'frameless-function':
                        $framelessFunctionInfos[] = new FramelessFunctionInfo($tag->getValue());
                        break;
                }
            }
        }

        $varNameSet = [];
        $args = [];
        $numRequiredArgs = 0;
        $foundVariadic = false;
        foreach ($func->getParams() as $i => $param) {
            if ($param->isPromoted()) {
                throw new Exception("Promoted properties are not supported");
            }

            $varName = $param->var->name;
            $preferRef = !empty($paramMeta[$varName]['prefer-ref']);
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
                if ($simpleType === null || !$simpleType->isMixed()) {
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
                $param->default ? $prettyPrinter->prettyPrintExpr($param->default) : null,
                AttributeInfo::createFromGroups($param->attrGroups)
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
            $supportsCompileTimeEval,
            $verify,
            $args,
            $return,
            $numRequiredArgs,
            $cond,
            $isUndocumentable,
            $minimumPhpVersionIdCompatibility,
            AttributeInfo::createFromGroups($func->attrGroups),
            $framelessFunctionInfos,
            ExposedDocComment::extractExposedComment($comments)
        );
    } catch (Exception $e) {
        throw new Exception($name . "(): " .$e->getMessage());
    }
}

/**
 * @param array<int, array<int, AttributeGroup> $attributes
 */
function parseConstLike(
    PrettyPrinterAbstract $prettyPrinter,
    AbstractConstName $name,
    Node\Const_ $const,
    int $flags,
    ?Node $type,
    array $comments,
    ?string $cond,
    bool $isUndocumentable,
    ?int $phpVersionIdMinimumCompatibility,
    array $attributes
): ConstInfo {
    $phpDocType = null;

    $tags = DocCommentTag::parseDocComments($comments);
    $tagMap = DocCommentTag::makeTagMap($tags);

    $deprecated = array_key_exists('deprecated', $tagMap);
    $isUndocumentable = $isUndocumentable || array_key_exists('undocumentable', $tagMap);
    $isFileCacheAllowed = !array_key_exists('no-file-cache', $tagMap);
    $cValue = $tagMap['cvalue'] ?? null;
    $link = $tagMap['link'] ?? null;

    foreach ($tags as $tag) {
        if ($tag->name === 'var') {
            $phpDocType = $tag->getType();
        }
    }

    if ($type === null && $phpDocType === null) {
        if ($const->value instanceof Node\Scalar\Float_) {
            $phpDocType = 'float';
        } elseif ($const->value instanceof Node\Scalar\Int_
            || ($const->value instanceof Expr\UnaryMinus
                && $const->value->expr instanceof Node\Scalar\Int_
            )
        ) {
            $phpDocType = 'int';
        } elseif ($const->value instanceof Node\Scalar\String_) {
            $phpDocType = 'string';
        } elseif ($const->value instanceof Expr\ConstFetch
            && $const->value->name instanceof Node\Name\FullyQualified
            && (
                $const->value->name->name === 'false'
                || $const->value->name->name === 'true'
            )
        ) {
            $phpDocType = 'bool';
        } elseif ($const->value instanceof Expr\ConstFetch
            && $const->value->name instanceof Node\Name\FullyQualified
            && $const->value->name->name === 'null'
        ) {
            $phpDocType = 'null';
        } else {
            throw new Exception("Missing type for constant " . $name->__toString());
        }
    }

    $constType = $type ? Type::fromNode($type) : null;
    $constPhpDocType = $phpDocType ? Type::fromString($phpDocType) : null;

    if ($const->value instanceof Expr\ConstFetch &&
        $const->value->name->toLowerString() === "null" &&
        $constType && !$constType->isNullable()
    ) {
        $simpleType = $constType->tryToSimpleType();
        if ($simpleType === null || !$simpleType->isMixed()) {
            throw new Exception("Constant " . $name->__toString() . " has null value, but is not nullable");
        }
    }

    return new ConstInfo(
        $name,
        $flags,
        $const->value,
        $prettyPrinter->prettyPrintExpr($const->value),
        $constType,
        $constPhpDocType,
        $deprecated,
        $cond,
        $cValue,
        $isUndocumentable,
        $link,
        $phpVersionIdMinimumCompatibility,
        $attributes,
        ExposedDocComment::extractExposedComment($comments),
        $isFileCacheAllowed
    );
}

/**
 * @param array<int, array<int, AttributeGroup> $attributes
 */
function parseProperty(
    Name $class,
    int $classFlags,
    int $flags,
    Stmt\PropertyProperty $property,
    ?Node $type,
    array $comments,
    PrettyPrinterAbstract $prettyPrinter,
    ?int $phpVersionIdMinimumCompatibility,
    array $attributes
): PropertyInfo {
    $phpDocType = null;

    $tags = DocCommentTag::parseDocComments($comments);
    $tagMap = DocCommentTag::makeTagMap($tags);

    $isDocReadonly = array_key_exists('readonly', $tagMap);
    $link = $tagMap['link'] ?? null;
    $isVirtual = array_key_exists('virtual', $tagMap);

    foreach ($tags as $tag) {
        if ($tag->name === 'var') {
            $phpDocType = $tag->getType();
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
        if ($simpleType === null || !$simpleType->isMixed()) {
            throw new Exception("Property $class::\$$property->name has null default, but is not nullable");
        }
    }

    return new PropertyInfo(
        new PropertyName($class, $property->name->__toString()),
        $classFlags,
        $flags,
        $propertyType,
        $phpDocType ? Type::fromString($phpDocType) : null,
        $property->default,
        $property->default ? $prettyPrinter->prettyPrintExpr($property->default) : null,
        $isDocReadonly,
        $isVirtual,
        $link,
        $phpVersionIdMinimumCompatibility,
        $attributes,
        ExposedDocComment::extractExposedComment($comments)
    );
}

/**
 * @param ConstInfo[] $consts
 * @param PropertyInfo[] $properties
 * @param FuncInfo[] $methods
 * @param EnumCaseInfo[] $enumCases
 */
function parseClass(
    Name $name,
    Stmt\ClassLike $class,
    array $consts,
    array $properties,
    array $methods,
    array $enumCases,
    ?string $cond,
    ?int $minimumPhpVersionIdCompatibility,
    bool $isUndocumentable
): ClassInfo {
    $comments = $class->getComments();
    $alias = null;
    $allowsDynamicProperties = false;

    $tags = DocCommentTag::parseDocComments($comments);
    $tagMap = DocCommentTag::makeTagMap($tags);

    $isDeprecated = array_key_exists('deprecated', $tagMap);
    $isStrictProperties = array_key_exists('strict-properties', $tagMap);
    $isNotSerializable = array_key_exists('not-serializable', $tagMap);
    $isUndocumentable = $isUndocumentable || array_key_exists('undocumentable', $tagMap);
    foreach ($tags as $tag) {
        if ($tag->name === 'alias') {
            $alias = $tag->getValue();
        }
    }

    $attributes = AttributeInfo::createFromGroups($class->attrGroups);
    foreach ($attributes as $attribute) {
        switch ($attribute->class) {
            case 'AllowDynamicProperties':
                $allowsDynamicProperties = true;
                break 2;
        }
    }

    if ($isStrictProperties && $allowsDynamicProperties) {
        throw new Exception("A class may not have '@strict-properties' and '#[\\AllowDynamicProperties]' at the same time.");
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

    if ($isUndocumentable) {
        foreach ($methods as $method) {
            $method->isUndocumentable = true;
        }
    }

    return new ClassInfo(
        $name,
        $class instanceof Class_ ? $class->flags : 0,
        $classKind,
        $alias,
        $class instanceof Enum_ && $class->scalarType !== null
            ? SimpleType::fromNode($class->scalarType) : null,
        $isDeprecated,
        $isStrictProperties,
        $attributes,
        ExposedDocComment::extractExposedComment($comments),
        $isNotSerializable,
        $extends,
        $implements,
        $consts,
        $properties,
        $methods,
        $enumCases,
        $cond,
        $minimumPhpVersionIdCompatibility,
        $isUndocumentable
    );
}

/**
 * @template T
 * @param iterable<T> $infos
 * @param Closure(T): string|null $codeGenerator
 * @param ?string $parentCond
 */
function generateCodeWithConditions(
    iterable $infos, string $separator, Closure $codeGenerator, ?string $parentCond = null): string {
    $code = "";

    // For combining the conditional blocks of the infos with the same condition
    $openCondition = null;
    foreach ($infos as $info) {
        $infoCode = $codeGenerator($info);
        if ($infoCode === null) {
            continue;
        }

        if ($info->cond && $info->cond !== $parentCond) {
            if ($openCondition !== null
                && $info->cond !== $openCondition
            ) {
                // Changing condition, end old
                $code .= "#endif\n";
                $code .= $separator;
                $code .= "#if {$info->cond}\n";
                $openCondition = $info->cond;
            } elseif ($openCondition === null) {
                // New condition with no existing one
                $code .= $separator;
                $code .= "#if {$info->cond}\n";
                $openCondition = $info->cond;
            } else {
                // Staying in the same condition
                $code .= $separator;
            }
            $code .= $infoCode;
        } else {
            if ($openCondition !== null) {
                // Ending the condition
                $code .= "#endif\n";
                $openCondition = null;
            }
            $code .= $separator;
            $code .= $infoCode;
        }
    }
    // The last info might have been in a conditional block
    if ($openCondition !== null) {
        $code .= "#endif\n";
    }

    return $code;
}

/**
 * @param array<string, ConstInfo> $allConstInfos
 */
function generateArgInfoCode(
    string $stubFilenameWithoutExtension,
    FileInfo $fileInfo,
    array $allConstInfos,
    string $stubHash
): string {
    $code = "/* This is a generated file, edit the .stub.php file instead.\n"
          . " * Stub hash: $stubHash */\n";

    $generatedFuncInfos = [];

    $argInfoCode = generateCodeWithConditions(
        $fileInfo->getAllFuncInfos(), "\n",
        static function (FuncInfo $funcInfo) use (&$generatedFuncInfos, $fileInfo) {
            /* If there already is an equivalent arginfo structure, only emit a #define */
            if ($generatedFuncInfo = $funcInfo->findEquivalent($generatedFuncInfos)) {
                $code = sprintf(
                    "#define %s %s\n",
                    $funcInfo->getArgInfoName(), $generatedFuncInfo->getArgInfoName()
                );
            } else {
                $code = $funcInfo->toArgInfoCode($fileInfo->getMinimumPhpVersionIdCompatibility());
            }

            $generatedFuncInfos[] = $funcInfo;
            return $code;
        }
    );

    if ($argInfoCode !== "") {
        $code .= "$argInfoCode\n";
    }

    if ($fileInfo->generateFunctionEntries) {
        $framelessFunctionCode = generateCodeWithConditions(
            $fileInfo->getAllFuncInfos(), "\n",
            static function (FuncInfo $funcInfo) {
                return $funcInfo->getFramelessDeclaration();
            }
        );

        if ($framelessFunctionCode !== "") {
            $code .= "$framelessFunctionCode\n";
        }

        $generatedFunctionDeclarations = [];
        $code .= generateCodeWithConditions(
            $fileInfo->getAllFuncInfos(), "",
            static function (FuncInfo $funcInfo) use ($fileInfo, &$generatedFunctionDeclarations) {
                $key = $funcInfo->getDeclarationKey();
                if (isset($generatedFunctionDeclarations[$key])) {
                    return null;
                }

                $generatedFunctionDeclarations[$key] = true;
                return $fileInfo->declarationPrefix . $funcInfo->getDeclaration();
            }
        );

        $code .= generateFunctionEntries(null, $fileInfo->funcInfos);

        foreach ($fileInfo->classInfos as $classInfo) {
            $code .= generateFunctionEntries($classInfo->name, $classInfo->funcInfos, $classInfo->cond);
        }
    }

    $php80MinimumCompatibility = $fileInfo->getMinimumPhpVersionIdCompatibility() === null || $fileInfo->getMinimumPhpVersionIdCompatibility() >= PHP_80_VERSION_ID;

    if ($fileInfo->generateClassEntries) {
        $declaredStrings = [];
        $attributeInitializationCode = generateFunctionAttributeInitialization($fileInfo->funcInfos, $allConstInfos, $fileInfo->getMinimumPhpVersionIdCompatibility(), null, $declaredStrings);
        $attributeInitializationCode .= generateGlobalConstantAttributeInitialization($fileInfo->constInfos, $allConstInfos, $fileInfo->getMinimumPhpVersionIdCompatibility(), null, $declaredStrings);
        if ($attributeInitializationCode) {
            if (!$php80MinimumCompatibility) {
                $attributeInitializationCode = "\n#if (PHP_VERSION_ID >= " . PHP_80_VERSION_ID . ")" . $attributeInitializationCode . "#endif\n";
            }
        }

        if ($attributeInitializationCode !== "" || !empty($fileInfo->constInfos)) {
            $code .= "\nstatic void register_{$stubFilenameWithoutExtension}_symbols(int module_number)\n";
            $code .= "{\n";

            $code .= generateCodeWithConditions(
                $fileInfo->constInfos,
                '',
                static fn (ConstInfo $constInfo): string => $constInfo->getDeclaration($allConstInfos)
            );

            if ($attributeInitializationCode !== "" && $fileInfo->constInfos) {
                $code .= "\n";
            }

            $code .= $attributeInitializationCode;
            $code .= "}\n";
        }

        $code .= $fileInfo->generateClassEntryCode($allConstInfos);
    }

    return $code;
}

/** @param FuncInfo[] $funcInfos */
function generateFunctionEntries(?Name $className, array $funcInfos, ?string $cond = null): string {
    // No need to add anything if there are no function entries
    if ($funcInfos === []) {
        return '';
    }

    $functionEntryName = "ext_functions";
    if ($className) {
        $underscoreName = implode("_", $className->getParts());
        $functionEntryName = "class_{$underscoreName}_methods";
    }

    $code = "\nstatic const zend_function_entry {$functionEntryName}[] = {\n";
    $code .= generateCodeWithConditions($funcInfos, "", static function (FuncInfo $funcInfo) {
        return $funcInfo->getFunctionEntry();
    }, $cond);
    $code .= "\tZEND_FE_END\n";
    $code .= "};\n";

    if ($cond) {
        $code = "\n#if {$cond}{$code}#endif\n";
    }

    return $code;
}

/**
 * @param iterable<FuncInfo> $funcInfos
 * @param array<string, string> &$declaredStrings Map of string content to
 *   the name of a zend_string already created with that content
 */
function generateFunctionAttributeInitialization(iterable $funcInfos, array $allConstInfos, ?int $phpVersionIdMinimumCompatibility, ?string $parentCond = null, array &$declaredStrings = []): string {
    return generateCodeWithConditions(
        $funcInfos,
        "",
        static function (FuncInfo $funcInfo) use ($allConstInfos, $phpVersionIdMinimumCompatibility, &$declaredStrings) {
            $code = null;

            if ($funcInfo->name instanceof MethodName) {
                $functionTable = "&class_entry->function_table";
            } else {
                $functionTable = "CG(function_table)";
            }

            // Make sure we don't try and use strings that might only be
            // conditionally available; string reuse is only among declarations
            // that are always there
            if ($funcInfo->cond) {
                $useDeclared = [];
            } else {
                $useDeclared = &$declaredStrings;
            }

            foreach ($funcInfo->attributes as $key => $attribute) {
                $code .= $attribute->generateCode(
                    "zend_add_function_attribute(zend_hash_str_find_ptr($functionTable, \"" . $funcInfo->name->getNameForAttributes() . "\", sizeof(\"" . $funcInfo->name->getNameForAttributes() . "\") - 1)",
                    "func_" . $funcInfo->name->getNameForAttributes() . "_$key",
                    $allConstInfos,
                    $phpVersionIdMinimumCompatibility,
                    $useDeclared
                );
            }

            foreach ($funcInfo->args as $index => $arg) {
                foreach ($arg->attributes as $key => $attribute) {
                    $code .= $attribute->generateCode(
                        "zend_add_parameter_attribute(zend_hash_str_find_ptr($functionTable, \"" . $funcInfo->name->getNameForAttributes() . "\", sizeof(\"" . $funcInfo->name->getNameForAttributes() . "\") - 1), $index",
                        "func_{$funcInfo->name->getNameForAttributes()}_arg{$index}_$key",
                        $allConstInfos,
                        $phpVersionIdMinimumCompatibility,
                        $useDeclared
                    );
                }
            }

            return $code;
        },
        $parentCond
    );
}

/**
 * @param iterable<ConstInfo> $constInfos
 * @param array<string, ConstInfo> $allConstInfos
 * @param array<string, string> &$declaredStrings Map of string content to
 *   the name of a zend_string already created with that content
 */
function generateGlobalConstantAttributeInitialization(
    iterable $constInfos,
    array $allConstInfos,
    ?int $phpVersionIdMinimumCompatibility,
    ?string $parentCond = null,
    array &$declaredStrings = []
): string {
    $isConditional = false;
    if ($phpVersionIdMinimumCompatibility !== null && $phpVersionIdMinimumCompatibility < PHP_85_VERSION_ID) {
        $isConditional = true;
    }
    $code = generateCodeWithConditions(
        $constInfos,
        "",
        static function (ConstInfo $constInfo) use ($allConstInfos, $isConditional, &$declaredStrings) {
            $code = "";

            if ($constInfo->attributes === []) {
                return null;
            }
            // Make sure we don't try and use strings that might only be
            // conditionally available; string reuse is only among declarations
            // that are always there
            if ($constInfo->cond) {
                $useDeclared = [];
            } else {
                $useDeclared = &$declaredStrings;
            }
            $constName = str_replace('\\', '\\\\', $constInfo->name->__toString());
            $constVarName = 'const_' . $constName;

            // The entire attribute block will be conditional if PHP < 8.5 is
            // supported, but also if PHP < 8.5 is supported we need to search
            // for the constant; see GH-19029
            if ($isConditional) {
                $code .= "\tzend_constant *$constVarName = zend_hash_str_find_ptr(EG(zend_constants), \"" . $constName . "\", sizeof(\"" . $constName . "\") - 1);\n";
            }
            foreach ($constInfo->attributes as $key => $attribute) {
                $code .= $attribute->generateCode(
                    "zend_add_global_constant_attribute($constVarName",
                    $constVarName . "_$key",
                    $allConstInfos,
                    PHP_85_VERSION_ID,
                    $useDeclared
                );
            }

            return $code;
        },
        $parentCond
    );
    if ($code && $isConditional) {
        return "\n#if (PHP_VERSION_ID >= " . PHP_85_VERSION_ID . ")\n" . $code . "#endif\n";
    }
    return $code;
}

/**
 * @param iterable<ConstInfo> $constInfos
 * @param array<string, ConstInfo> $allConstInfos
 * @param array<string, string> &$declaredStrings Map of string content to
 *    the name of a zend_string already created with that content
 */
function generateConstantAttributeInitialization(
    iterable $constInfos,
    array $allConstInfos,
    ?int $phpVersionIdMinimumCompatibility,
    ?string $parentCond = null,
    array &$declaredStrings = []
): string {
    return generateCodeWithConditions(
        $constInfos,
        "",
        static function (ConstInfo $constInfo) use ($allConstInfos, $phpVersionIdMinimumCompatibility, &$declaredStrings) {
            $code = null;

            // Make sure we don't try and use strings that might only be
            // conditionally available; string reuse is only among declarations
            // that are always there
            if ($constInfo->cond) {
                $useDeclared = [];
            } else {
                $useDeclared = &$declaredStrings;
            }
            foreach ($constInfo->attributes as $key => $attribute) {
                $code .= $attribute->generateCode(
                    "zend_add_class_constant_attribute(class_entry, const_" . $constInfo->name->getDeclarationName(),
                    "const_" . $constInfo->name->getDeclarationName() . "_$key",
                    $allConstInfos,
                    $phpVersionIdMinimumCompatibility,
                    $useDeclared
                );
            }

            return $code;
        },
        $parentCond
    );
}

/**
 * @param iterable<PropertyInfo> $propertyInfos
 * @param array<string, ConstInfo> $allConstInfos
 * @param array<string, string> &$declaredStrings Map of string content to
 *    the name of a zend_string already created with that content
 */
function generatePropertyAttributeInitialization(
    iterable $propertyInfos,
    array $allConstInfos,
    ?int $phpVersionIdMinimumCompatibility,
    array &$declaredStrings
): string {
    $code = "";
    foreach ($propertyInfos as $propertyInfo) {
        foreach ($propertyInfo->attributes as $key => $attribute) {
            $code .= $attribute->generateCode(
                "zend_add_property_attribute(class_entry, property_" . $propertyInfo->name->getDeclarationName(),
                "property_" . $propertyInfo->name->getDeclarationName() . "_" . $key,
                $allConstInfos,
                $phpVersionIdMinimumCompatibility,
                $declaredStrings
            );
        }
    }

    return $code;
}

/** @param array<string, FuncInfo> $funcMap */
function generateOptimizerInfo(array $funcMap): string {
    $code = "/* This is a generated file, edit the .stub.php files instead. */\n\n";

    $code .= "static const func_info_t func_infos[] = {\n";

    $code .= generateCodeWithConditions($funcMap, "", static function (FuncInfo $funcInfo) {
        return $funcInfo->getOptimizerInfo();
    });

    $code .= "};\n";

    return $code;
}

/**
 * @param array<string, ConstInfo> $constMap
 * @param array<string, ConstInfo> $undocumentedConstMap
 * @return array<string, string|null>
 */
function replacePredefinedConstants(string $targetDirectory, array $constMap, array &$undocumentedConstMap): array {
    /** @var array<string, string> $documentedConstMap */
    $documentedConstMap = [];
    /** @var array<string, string> $predefinedConstants */
    $predefinedConstants = [];

    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($targetDirectory),
        RecursiveIteratorIterator::LEAVES_ONLY
    );

    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (!preg_match('/(?:[\w\.]*constants[\w\._]*|tokens).xml$/i', basename($pathName))) {
            continue;
        }

        $xml = file_get_contents($pathName);
        if ($xml === false) {
            continue;
        }

        if (stripos($xml, "<appendix") === false && stripos($xml, "<sect2") === false &&
            stripos($xml, "<chapter") === false && stripos($xml, 'role="constant_list"') === false
        ) {
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

        $updated = false;

        foreach ($doc->getElementsByTagName("varlistentry") as $entry) {
            if (!$entry instanceof DOMElement) {
                continue;
            }

            foreach ($entry->getElementsByTagName("term") as $manualTermElement) {
                $manualConstantElement = $manualTermElement->getElementsByTagName("constant")->item(0);
                if (!$manualConstantElement instanceof DOMElement) {
                    continue;
                }

                $manualConstantName = $manualConstantElement->textContent;

                $stubConstant = $constMap[$manualConstantName] ?? null;
                if ($stubConstant === null) {
                    continue;
                }

                $documentedConstMap[$manualConstantName] = $manualConstantName;

                if ($entry->firstChild instanceof DOMText) {
                    $indentationLevel = strlen(str_replace("\n", "", $entry->firstChild->textContent));
                } else {
                    $indentationLevel = 3;
                }
                $newTermElement = $stubConstant->getPredefinedConstantTerm($doc, $indentationLevel);

                if ($manualTermElement->textContent === $newTermElement->textContent) {
                    continue;
                }

                $manualTermElement->parentNode->replaceChild($newTermElement, $manualTermElement);
                $updated = true;
            }
        }

        foreach ($doc->getElementsByTagName("row") as $row) {
            if (!$row instanceof DOMElement) {
                continue;
            }

            $entry = $row->getElementsByTagName("entry")->item(0);
            if (!$entry instanceof DOMElement) {
                continue;
            }

            foreach ($entry->getElementsByTagName("constant") as $manualConstantElement) {
                if (!$manualConstantElement instanceof DOMElement) {
                    continue;
                }

                $manualConstantName = $manualConstantElement->textContent;

                $stubConstant = $constMap[$manualConstantName] ?? null;
                if ($stubConstant === null) {
                    continue;
                }

                $documentedConstMap[$manualConstantName] = $manualConstantName;

                if ($row->firstChild instanceof DOMText) {
                    $indentationLevel = strlen(str_replace("\n", "", $row->firstChild->textContent));
                } else {
                    $indentationLevel = 3;
                }
                $newEntryElement = $stubConstant->getPredefinedConstantEntry($doc, $indentationLevel);

                if ($entry->textContent === $newEntryElement->textContent) {
                    continue;
                }

                $entry->parentNode->replaceChild($newEntryElement, $entry);
                $updated = true;
            }
        }

        if ($updated) {
            $replacedXml = $doc->saveXML();

            $replacedXml = preg_replace(
                [
                    "/REPLACED-ENTITY-([A-Za-z0-9._{}%-]+?;)/",
                    '/<appendix\s+xmlns="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<appendix\s+xmlns="([^"]+)"\s+xmlns:xlink="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<sect2\s+xmlns="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<sect2\s+xmlns="([^"]+)"\s+xmlns:xlink="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<chapter\s+xmlns="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<chapter\s+xmlns="([^"]+)"\s+xmlns:xlink="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                ],
                [
                    "&$1",
                    "<appendix xml:id=\"$2\" xmlns=\"$1\">",
                    "<appendix xml:id=\"$3\" xmlns=\"$1\" xmlns:xlink=\"$2\">",
                    "<sect2 xml:id=\"$2\" xmlns=\"$1\">",
                    "<sect2 xml:id=\"$3\" xmlns=\"$1\" xmlns:xlink=\"$2\">",
                    "<chapter xml:id=\"$2\" xmlns=\"$1\">",
                    "<chapter xml:id=\"$3\" xmlns=\"$1\" xmlns:xlink=\"$2\">",
                ],
                $replacedXml
            );

            $predefinedConstants[$pathName] = $replacedXml;
        }
    }

    $undocumentedConstMap = array_diff_key($constMap, $documentedConstMap);

    return $predefinedConstants;
}

/**
 * @param array<string, ClassInfo> $classMap
 * @param array<string, ConstInfo> $allConstInfos
 * @return array<string, string>
 */
function generateClassSynopses(array $classMap, array $allConstInfos): array {
    $result = [];

    foreach ($classMap as $classInfo) {
        $classSynopsis = $classInfo->getClassSynopsisDocument($classMap, $allConstInfos);
        if ($classSynopsis !== null) {
            $result[ClassInfo::getClassSynopsisFilename($classInfo->name) . ".xml"] = $classSynopsis;
        }
    }

    return $result;
}

/**
 * @param array<string, ClassInfo> $classMap
 * @param array<string, ConstInfo> $allConstInfos
 * @param array<string, ClassInfo> $undocumentedClassMap
 * @return array<string, string>
 */
function replaceClassSynopses(
    string $targetDirectory,
    array $classMap,
    array $allConstInfos,
    array &$undocumentedClassMap
): array {
    /** @var array<string, string> $documentedClassMap */
    $documentedClassMap = [];
    /** @var array<string, string> $classSynopses */
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

            $child = $classSynopsis->firstElementChild;
            if ($child === null) {
                continue;
            }
            $child = $child->lastElementChild;
            if ($child === null) {
                continue;
            }
            $className = $child->textContent;
            if (!isset($classMap[$className])) {
                continue;
            }

            $documentedClassMap[$className] = $className;

            $classInfo = $classMap[$className];

            $newClassSynopsis = $classInfo->getClassSynopsisElement($doc, $classMap, $allConstInfos);
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
                    '/<reference\s+role="(\w+)"\s+xmlns="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<reference\s+role="(\w+)"\s+xmlns="([^"]+)"\s+xmlns:xi="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<reference\s+role="(\w+)"\s+xmlns="([^"]+)"\s+xmlns:xlink="([^"]+)"\s+xmlns:xi="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<reference\s+role="(\w+)"\s+xmlns:xlink="([^"]+)"\s+xmlns:xi="([^"]+)"\s+xmlns="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<reference\s+xmlns=\"([^"]+)\"\s+xmlns:xlink="([^"]+)"\s+xmlns:xi="([^"]+)"\s+role="(\w+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<reference\s+xmlns=\"([^"]+)\"\s+xmlns:xlink="([^"]+)"\s+xmlns:xi="([^"]+)"\s+xml:id="([^"]+)"\s+role="(\w+)"\s*>/i',
                ],
                [
                    "&$1",
                    "<reference xml:id=\"$3\" role=\"$1\" xmlns=\"$2\">",
                    "<reference xml:id=\"$4\" role=\"$1\" xmlns=\"$2\" xmlns:xi=\"$3\">",
                    "<reference xml:id=\"$5\" role=\"$1\" xmlns=\"$2\" xmlns:xlink=\"$3\" xmlns:xi=\"$4\">",
                    "<reference xml:id=\"$5\" role=\"$1\" xmlns=\"$4\" xmlns:xlink=\"$2\" xmlns:xi=\"$2\">",
                    "<reference xml:id=\"$5\" role=\"$4\" xmlns=\"$1\" xmlns:xlink=\"$2\" xmlns:xi=\"$3\">",
                    "<reference xml:id=\"$4\" role=\"$5\" xmlns=\"$1\" xmlns:xlink=\"$2\" xmlns:xi=\"$3\">",
                ],
                $replacedXml
            );

            $classSynopses[$pathName] = $replacedXml;
        }
    }

    $undocumentedClassMap = array_diff_key($classMap, $documentedClassMap);

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
 * @return array<string, string>
 */
function generateMethodSynopses(array $funcMap): array {
    $result = [];

    foreach ($funcMap as $funcInfo) {
        $methodSynopsis = $funcInfo->getMethodSynopsisDocument();
        if ($methodSynopsis !== null) {
            $result[$funcInfo->name->getMethodSynopsisFilename() . ".xml"] = $methodSynopsis;
        }
    }

    return $result;
}

/**
 * @param array<string, FuncInfo> $funcMap
 * @param array<int, string> $methodSynopsisWarnings
 * @param array<string, FuncInfo> $undocumentedFuncMap
 * @return array<string, string>
 */
function replaceMethodSynopses(
    string $targetDirectory,
    array $funcMap,
    bool $isVerifyManual,
    array &$methodSynopsisWarnings,
    array &$undocumentedFuncMap
): array {
    /** @var array<string, string> $documentedFuncMap */
    $documentedFuncMap = [];
    /** @var array<string, string> $methodSynopses */
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

        if ($isVerifyManual) {
            $matches = [];
            preg_match("/<refname>\s*([\w:]+)\s*<\/refname>\s*<refpurpose>\s*&Alias;\s*<(?:function|methodname)>\s*([\w:]+)\s*<\/(?:function|methodname)>\s*<\/refpurpose>/i", $xml, $matches);
            $aliasName = $matches[1] ?? null;
            $alias = $funcMap[$aliasName] ?? null;
            $funcName = $matches[2] ?? null;
            $func = $funcMap[$funcName] ?? null;

            if ($alias &&
                !$alias->isUndocumentable &&
                ($func === null || $func->alias === null || $func->alias->__toString() !== $aliasName) &&
                ($alias->alias === null || $alias->alias->__toString() !== $funcName)
            ) {
                $methodSynopsisWarnings[] = "$aliasName()" . ($alias->alias ? " is an alias of " . $alias->alias->__toString() . "(), but it" : "") . " is incorrectly documented as an alias for $funcName()";
            }

            $matches = [];
            preg_match("/<(?:para|simpara)>\s*(?:&info.function.alias;|&info.method.alias;|&Alias;)\s+<(?:function|methodname)>\s*([\w:]+)\s*<\/(?:function|methodname)>/i", $xml, $matches);
            $descriptionFuncName = $matches[1] ?? null;
            $descriptionFunc = $funcMap[$descriptionFuncName] ?? null;
            if ($descriptionFunc && $funcName !== $descriptionFuncName) {
                $methodSynopsisWarnings[] = "Alias in the method synopsis description of $pathName doesn't match the alias in the <refpurpose>";
            }

            if ($aliasName) {
                $documentedFuncMap[$aliasName] = $aliasName;
            }
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

            $item = $methodSynopsis->getElementsByTagName("methodname")->item(0);
            if (!$item instanceof DOMElement) {
                continue;
            }
            $funcName = $item->textContent;
            if (!isset($funcMap[$funcName])) {
                continue;
            }

            $funcInfo = $funcMap[$funcName];
            $documentedFuncMap[$funcInfo->name->__toString()] = $funcInfo->name->__toString();

            $newMethodSynopsis = $funcInfo->getMethodSynopsisElement($doc);
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
                    '/<refentry\s+xmlns="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<refentry\s+xmlns="([^"]+)"\s+xmlns:xlink="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
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

    $undocumentedFuncMap = array_diff_key($funcMap, $documentedFuncMap);

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
        $downloadUrl = "https://github.com/nikic/PHP-Parser/archive/$tarName";
        passthru("wget -O $tarName $downloadUrl", $exit);
        if ($exit !== 0) {
            passthru("curl -LO $downloadUrl", $exit);
        }
        if ($exit !== 0) {
            throw new Exception("Failed to download PHP-Parser tarball");
        }
        if (!mkdir($phpParserDir)) {
            throw new Exception("Failed to create directory $phpParserDir");
        }
        passthru("tar xvzf $tarName -C PHP-Parser-$version --strip-components 1", $exit);
        if ($exit !== 0) {
            rmdir($phpParserDir);
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
    $version = "5.6.1";
    $phpParserDir = __DIR__ . "/PHP-Parser-$version";
    if (!is_dir($phpParserDir)) {
        installPhpParser($version, $phpParserDir);
    }

    spl_autoload_register(static function(string $class) use ($phpParserDir) {
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
        "force-regeneration", "parameter-stats", "help", "verify", "verify-manual", "replace-predefined-constants",
        "generate-classsynopses", "replace-classsynopses", "generate-methodsynopses", "replace-methodsynopses",
        "generate-optimizer-info",
    ],
    $optind
);

$context = new Context;
$printParameterStats = isset($options["parameter-stats"]);
$verify = isset($options["verify"]);
$verifyManual = isset($options["verify-manual"]);
$replacePredefinedConstants = isset($options["replace-predefined-constants"]);
$generateClassSynopses = isset($options["generate-classsynopses"]);
$replaceClassSynopses = isset($options["replace-classsynopses"]);
$generateMethodSynopses = isset($options["generate-methodsynopses"]);
$replaceMethodSynopses = isset($options["replace-methodsynopses"]);
$generateOptimizerInfo = isset($options["generate-optimizer-info"]);
$context->forceRegeneration = isset($options["f"]) || isset($options["force-regeneration"]);
$context->forceParse = $context->forceRegeneration || $printParameterStats || $verify || $verifyManual || $replacePredefinedConstants || $generateClassSynopses || $generateOptimizerInfo || $replaceClassSynopses || $generateMethodSynopses || $replaceMethodSynopses;

if (isset($options["h"]) || isset($options["help"])) {
    die("\nUsage: gen_stub.php [ -f | --force-regeneration ] [ --replace-predefined-constants ] [ --generate-classsynopses ] [ --replace-classsynopses ] [ --generate-methodsynopses ] [ --replace-methodsynopses ] [ --parameter-stats ] [ --verify ]  [ --verify-manual ] [ --generate-optimizer-info ] [ -h | --help ] [ name.stub.php | directory ] [ directory ]\n\n");
}

$locations = array_slice($argv, $optind);
$locationCount = count($locations);
if ($replacePredefinedConstants && $locationCount < 2) {
    die("At least one source stub path and a target manual directory has to be provided:\n./build/gen_stub.php --replace-predefined-constants ./ ../doc-en/\n");
}
if ($replaceClassSynopses && $locationCount < 2) {
    die("At least one source stub path and a target manual directory has to be provided:\n./build/gen_stub.php --replace-classsynopses ./ ../doc-en/\n");
}
if ($generateMethodSynopses && $locationCount < 2) {
    die("At least one source stub path and a target manual directory has to be provided:\n./build/gen_stub.php --generate-methodsynopses ./ ../doc-en/\n");
}
if ($replaceMethodSynopses && $locationCount < 2) {
    die("At least one source stub path and a target manual directory has to be provided:\n./build/gen_stub.php --replace-methodsynopses ./ ../doc-en/\n");
}
if ($verifyManual && $locationCount < 2) {
    die("At least one source stub path and a target manual directory has to be provided:\n./build/gen_stub.php --verify-manual ./ ../doc-en/\n");
}
$manualTarget = null;
if ($replacePredefinedConstants || $replaceClassSynopses || $generateMethodSynopses || $replaceMethodSynopses || $verifyManual) {
    $manualTarget = array_pop($locations);
}
if ($locations === []) {
    $locations = ['.'];
}

$fileInfos = [];
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

/** @var array<string, ConstInfo> $undocumentedConstMap */
$undocumentedConstMap = [];
/** @var array<string, ClassInfo> $undocumentedClassMap */
$undocumentedClassMap = [];
/** @var array<string, FuncInfo> $undocumentedFuncMap */
$undocumentedFuncMap = [];
/** @var array<int, string> $methodSynopsisWarnings */
$methodSynopsisWarnings = [];

foreach ($fileInfos as $fileInfo) {
    foreach ($fileInfo->getAllFuncInfos() as $funcInfo) {
        $funcMap[$funcInfo->name->__toString()] = $funcInfo;

        // TODO: Don't use aliasMap for methodsynopsis?
        if ($funcInfo->aliasType === "alias") {
            $aliasMap[$funcInfo->alias->__toString()] = $funcInfo;
        }
    }

    foreach ($fileInfo->classInfos as $classInfo) {
        $classMap[$classInfo->name->__toString()] = $classInfo;

        if ($classInfo->alias !== null) {
            $classMap[$classInfo->alias] = $classInfo;
        }
    }
}

if ($verify) {
    $errors = [];

    foreach ($funcMap as $aliasFunc) {
        if (!$aliasFunc->alias || $aliasFunc->aliasType !== "alias") {
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

if ($replacePredefinedConstants || $verifyManual) {
    $predefinedConstants = replacePredefinedConstants($manualTarget, $context->allConstInfos, $undocumentedConstMap);

    if ($replacePredefinedConstants) {
        foreach ($predefinedConstants as $filename => $content) {
            reportFilePutContents($filename, $content);
        }
    }
}

if ($generateClassSynopses) {
    $classSynopsesDirectory = getcwd() . "/classsynopses";

    $classSynopses = generateClassSynopses($classMap, $context->allConstInfos);
    if (!empty($classSynopses)) {
        if (!file_exists($classSynopsesDirectory)) {
            mkdir($classSynopsesDirectory);
        }

        foreach ($classSynopses as $filename => $content) {
            reportFilePutContents("$classSynopsesDirectory/$filename", $content);
        }
    }
}

if ($replaceClassSynopses || $verifyManual) {
    $classSynopses = replaceClassSynopses($manualTarget, $classMap, $context->allConstInfos, $undocumentedClassMap);

    if ($replaceClassSynopses) {
        foreach ($classSynopses as $filename => $content) {
            reportFilePutContents($filename, $content);
        }
    }
}

if ($generateMethodSynopses) {
    $methodSynopses = generateMethodSynopses($funcMap);
    if (!file_exists($manualTarget)) {
        mkdir($manualTarget);
    }

    foreach ($methodSynopses as $filename => $content) {
        $path = "$manualTarget/$filename";

        if (!file_exists($path)) {
            if (!file_exists(dirname($path))) {
                mkdir(dirname($path));
            }

            reportFilePutContents($path, $content);
        }
    }
}

if ($replaceMethodSynopses || $verifyManual) {
    $methodSynopses = replaceMethodSynopses($manualTarget, $funcMap, $verifyManual, $methodSynopsisWarnings, $undocumentedFuncMap);

    if ($replaceMethodSynopses) {
        foreach ($methodSynopses as $filename => $content) {
            reportFilePutContents($filename, $content);
        }
    }
}

if ($generateOptimizerInfo) {
    $filename = dirname(__FILE__, 2) . "/Zend/Optimizer/zend_func_infos.h";
    $optimizerInfo = generateOptimizerInfo($funcMap);

    reportFilePutContents($filename, $optimizerInfo);
}

if ($verifyManual) {
    foreach ($undocumentedConstMap as $constName => $info) {
        if ($info->name instanceof ClassConstName || $info->isUndocumentable) {
            continue;
        }

        echo "Warning: Missing predefined constant for $constName\n";
    }

    foreach ($methodSynopsisWarnings as $warning) {
        echo "Warning: $warning\n";
    }

    foreach ($undocumentedClassMap as $className => $info) {
        if (!$info->isUndocumentable) {
            echo "Warning: Missing class synopsis for $className\n";
        }
    }

    foreach ($undocumentedFuncMap as $functionName => $info) {
        if (!$info->isUndocumentable) {
            echo "Warning: Missing method synopsis for $functionName()\n";
        }
    }
}
